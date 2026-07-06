#include <QtTest>
#include <QtWidgets>

#include "mainwindow.h"

class Testlab1 : public QObject
{
    Q_OBJECT

private slots:
    void testCorrectAnswer()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit = window.findChild<QLineEdit *>("l1i");
        QPushButton *button = window.findChild<QPushButton *>("l1b");
        QLabel *label = window.findChild<QLabel *>("l1r");

        QVERIFY(lineEdit);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "10");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Площадь круга: 314.159"));
    }

    void testWrongAnswer_1()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit = window.findChild<QLineEdit *>("l1i");
        QPushButton *button = window.findChild<QPushButton *>("l1b");
        QLabel *label = window.findChild<QLabel *>("l1r");

        QVERIFY(lineEdit);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "-10");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Ошибка: введите число >= 0"));
    }

    void testWrongAnswer_2()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit = window.findChild<QLineEdit *>("l1i");
        QPushButton *button = window.findChild<QPushButton *>("l1b");
        QLabel *label = window.findChild<QLabel *>("l1r");

        QVERIFY(lineEdit);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "something");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Ошибка: введите число >= 0"));
    }
};

QTEST_MAIN(Testlab1)
#include "test_lab1.moc"