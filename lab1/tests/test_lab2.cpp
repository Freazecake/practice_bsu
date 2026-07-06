#include <QtTest>
#include <QtWidgets>

#include "mainwindow.h"

class Testlab2 : public QObject
{
    Q_OBJECT

private slots:
    void testCorrectAnswer()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit = window.findChild<QLineEdit *>("l2i");
        QPushButton *button = window.findChild<QPushButton *>("l2b");
        QLabel *label = window.findChild<QLabel *>("l2r");

        QVERIFY(lineEdit);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "i have to tell you something");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: something have to tell you i"));
    }

    void testWrongAnswer_1()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit = window.findChild<QLineEdit *>("l2i");
        QPushButton *button = window.findChild<QPushButton *>("l2b");
        QLabel *label = window.findChild<QLabel *>("l2r");

        QVERIFY(lineEdit);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: No words found"));
    }

    void testWrongAnswer_2()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit = window.findChild<QLineEdit *>("l2i");
        QPushButton *button = window.findChild<QPushButton *>("l2b");
        QLabel *label = window.findChild<QLabel *>("l2r");

        QVERIFY(lineEdit);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "something");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: Only one word (shortest == longest). Nothing to swap"));
    }
};

QTEST_MAIN(Testlab2)
#include "test_lab2.moc"