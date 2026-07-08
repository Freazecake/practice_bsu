#ifndef TST_SCENE_H
#define TST_SCENE_H

#include <QObject>

// Отдельный тестовый класс — тестирует фигуры и сцену без GUI (без QApplication)
class TestScene : public QObject
{
    Q_OBJECT

private slots:
    // Создание объектов и геометрия фигур
    void testCreateAndAddShapes();
    void testLineHitTest();
    void testRectangleHitTest();
    void testCircleHitTest();

    // Работа с коллекцией объектов
    void testRemoveShape();
    void testHitTestTopmostRespectsLayerOrder();
    void testLayerOrdering();
    void testMoveShape();

    // Сериализация (используется и для файлов, и для undo/copy-paste)
    void testSerializeRoundTrip();
    void testSaveAndLoadFile();

    // Обработка ошибок
    void testLoadCorruptedFileFails();
    void testLoadMissingFileFails();
};

#endif // TST_SCENE_H
