#include "tst_scene.h"
#include "../scene.h"
#include "../shapes.h"

#include <QtTest>
#include <QTemporaryDir>

void TestScene::testCreateAndAddShapes()
{
    Scene scene;
    QCOMPARE(scene.count(), 0);

    auto *line = new LineShape();
    line->points() = { QPointF(0, 0), QPointF(10, 10) };
    scene.addShape(line);

    auto *rect = new RectangleShape();
    rect->points() = { QPointF(0, 0), QPointF(50, 30) };
    scene.addShape(rect);

    QCOMPARE(scene.count(), 2);
    QCOMPARE(scene.at(0)->type(), ShapeType::Line);
    QCOMPARE(scene.at(1)->type(), ShapeType::Rectangle);
}

void TestScene::testLineHitTest()
{
    auto *line = new LineShape();
    line->points() = { QPointF(0, 0), QPointF(100, 0) };
    line->setLineWidth(2);

    QVERIFY(line->hitTest(QPointF(50, 0)));   // точка прямо на линии
    QVERIFY(line->hitTest(QPointF(50, 3)));   // рядом с линией (в пределах допуска)
    QVERIFY(!line->hitTest(QPointF(50, 50))); // далеко от линии
    delete line;
}

void TestScene::testRectangleHitTest()
{
    auto *rect = new RectangleShape();
    rect->points() = { QPointF(10, 10), QPointF(110, 60) };
    rect->setFillColor(QColor(0, 0, 0, 0)); // без заливки — попадание только по контуру

    QVERIFY(rect->hitTest(QPointF(10, 30)));   // на левой границе
    QVERIFY(!rect->hitTest(QPointF(60, 35)));  // внутри, но не залито — мимо

    rect->setFillColor(Qt::red); // с заливкой — попадание в любую точку внутри
    QVERIFY(rect->hitTest(QPointF(60, 35)));
    QVERIFY(!rect->hitTest(QPointF(200, 200))); // далеко за пределами
    delete rect;
}

void TestScene::testCircleHitTest()
{
    auto *circle = new CircleShape();
    circle->points() = { QPointF(100, 100), QPointF(150, 100) }; // центр + точка на окружности, радиус 50
    QCOMPARE(circle->radius(), 50.0);

    circle->setFillColor(QColor(0, 0, 0, 0));
    QVERIFY(circle->hitTest(QPointF(150, 100))); // на границе
    QVERIFY(!circle->hitTest(QPointF(100, 100))); // в центре, но не залито

    circle->setFillColor(Qt::blue);
    QVERIFY(circle->hitTest(QPointF(100, 100))); // теперь центр тоже попадает (заливка)
    delete circle;
}

void TestScene::testRemoveShape()
{
    Scene scene;
    scene.addShape(new LineShape());
    scene.addShape(new RectangleShape());
    scene.addShape(new EllipseShape());
    QCOMPARE(scene.count(), 3);

    bool ok = scene.removeAt(1);
    QVERIFY(ok);
    QCOMPARE(scene.count(), 2);
    QCOMPARE(scene.at(0)->type(), ShapeType::Line);
    QCOMPARE(scene.at(1)->type(), ShapeType::Ellipse);

    QVERIFY(!scene.removeAt(99)); // некорректный индекс
}

void TestScene::testHitTestTopmostRespectsLayerOrder()
{
    Scene scene;

    auto *back = new RectangleShape();
    back->points() = { QPointF(0, 0), QPointF(100, 100) };
    back->setFillColor(Qt::red);
    scene.addShape(back);

    auto *front = new RectangleShape();
    front->points() = { QPointF(20, 20), QPointF(80, 80) };
    front->setFillColor(Qt::blue);
    scene.addShape(front); // добавлен позже => отрисовывается поверх => должен находиться первым

    int hit = scene.hitTestTopmost(QPointF(50, 50));
    QCOMPARE(hit, 1); // верхний (front) должен быть найден, а не back
}

void TestScene::testLayerOrdering()
{
    Scene scene;
    scene.addShape(new LineShape());     // 0
    scene.addShape(new RectangleShape());// 1
    scene.addShape(new EllipseShape());  // 2

    QVERIFY(scene.sendToBack(2));
    QCOMPARE(scene.at(0)->type(), ShapeType::Ellipse);

    QVERIFY(scene.bringToFront(0));
    QCOMPARE(scene.at(scene.count() - 1)->type(), ShapeType::Ellipse);

    QVERIFY(scene.lower(scene.count() - 1));
    QCOMPARE(scene.at(scene.count() - 1)->type(), ShapeType::Rectangle);
}

void TestScene::testMoveShape()
{
    auto *line = new LineShape();
    line->points() = { QPointF(0, 0), QPointF(10, 10) };
    line->moveBy(QPointF(5, 5));

    QCOMPARE(line->points()[0], QPointF(5, 5));
    QCOMPARE(line->points()[1], QPointF(15, 15));
    delete line;
}

void TestScene::testSerializeRoundTrip()
{
    Scene scene;
    auto *rect = new RectangleShape();
    rect->points() = { QPointF(1, 2), QPointF(50, 60) };
    rect->setStrokeColor(Qt::darkGreen);
    rect->setFillColor(Qt::yellow);
    rect->setLineWidth(4);
    scene.addShape(rect);

    QByteArray data = scene.toByteArray();

    Scene loaded;
    QString error;
    QVERIFY2(loaded.fromByteArray(data, &error), qPrintable(error));

    QCOMPARE(loaded.count(), 1);
    QCOMPARE(loaded.at(0)->type(), ShapeType::Rectangle);
    QCOMPARE(loaded.at(0)->strokeColor(), QColor(Qt::darkGreen));
    QCOMPARE(loaded.at(0)->fillColor(), QColor(Qt::yellow));
    QCOMPARE(loaded.at(0)->lineWidth(), 4);
    QCOMPARE(loaded.at(0)->points(), rect->points());
}

void TestScene::testSaveAndLoadFile()
{
    Scene scene;
    scene.addShape(new LineShape());
    auto *poly = new PolygonShape();
    poly->points() = { QPointF(0, 0), QPointF(10, 0), QPointF(5, 10) };
    scene.addShape(poly);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("scene.vge");

    QString error;
    QVERIFY2(scene.saveToFile(path, &error), qPrintable(error));
    QVERIFY(QFile::exists(path));

    Scene loaded;
    QVERIFY2(loaded.loadFromFile(path, &error), qPrintable(error));
    QCOMPARE(loaded.count(), 2);
    QCOMPARE(loaded.at(1)->type(), ShapeType::Polygon);
    QCOMPARE(loaded.at(1)->points().size(), 3);
}

void TestScene::testLoadCorruptedFileFails()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("bad.vge");

    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("это не файл векторного редактора");
    }

    Scene scene;
    QString error;
    QVERIFY(!scene.loadFromFile(path, &error));
    QVERIFY(!error.isEmpty());
}

void TestScene::testLoadMissingFileFails()
{
    Scene scene;
    QString error;
    QVERIFY(!scene.loadFromFile("/no/such/path/missing_12345.vge", &error));
    QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(TestScene)
