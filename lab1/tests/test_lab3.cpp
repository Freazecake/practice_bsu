#include <QtTest>
#include <QtWidgets>
#include <iostream>

#include "mainwindow.h"

class Testlab3 : public QObject
{
    Q_OBJECT

private slots:
    void testCorrectAnswer()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(button);

        QTest::keyClicks(lineEdit1, "");
        QTest::keyClicks(lineEdit2, "i have to tell you something");

        QTest::mouseClick(button, Qt::LeftButton);

        std::cout << label->text();
        QCOMPARE(label->text(), QString());
    }

    void testWrongAnswer_1()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: Ошибка: Введите дроби в формате 'числитель/знаменатель'."));
    }

    void testWrongAnswer_2()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(button);

        QTest::keyClicks(lineEdit, "something");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: Only one word (shortest == longest). Nothing to swap"));
    }
};

QTEST_MAIN(Testlab3)
#include "test_lab3.moc"