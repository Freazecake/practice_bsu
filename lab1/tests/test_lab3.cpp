#include <QtTest>
#include <QtWidgets>
#include <iostream>

#include "mainwindow.h"

class Testlab3 : public QObject
{
    Q_OBJECT

private slots:
    void testCorrectAnswer_Addition()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QComboBox *operation = window.findChild<QComboBox *>("l3op");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(operation);
        QVERIFY(button);
        QVERIFY(label);

        QTest::keyClicks(lineEdit1, "1/2");
        QTest::keyClicks(lineEdit2, "1/4");
        operation->setCurrentIndex(0); // Сложение

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: 1/2 + 1/4 = 3/4"));
    }

    void testCorrectAnswer_Reduction()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QComboBox *operation = window.findChild<QComboBox *>("l3op");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(operation);
        QVERIFY(button);
        QVERIFY(label);

        QTest::keyClicks(lineEdit1, "4/2"); // должно сократиться до 2/1
        QTest::keyClicks(lineEdit2, "1/1");
        operation->setCurrentIndex(2); // Умножение

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: 2/1 * 1/1 = 2/1"));
    }

    void testCorrectAnswer_UnaryMinus()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QComboBox *operation = window.findChild<QComboBox *>("l3op");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(operation);
        QVERIFY(button);
        QVERIFY(label);

        QTest::keyClicks(lineEdit1, "3/4");
        operation->setCurrentIndex(4); // Унарный минус (вторая дробь не нужна)

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: -(3/4) = -3/4"));
    }

    void testWrongAnswer_EmptyFirstField()
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
        QVERIFY(label);

        QTest::keyClicks(lineEdit1, "");
        QTest::keyClicks(lineEdit2, "1/2");

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(),
                 QString("Результат: Ошибка: Введите первую дробь в формате 'числитель/знаменатель'."));
    }

    void testWrongAnswer_InvalidSecondField()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QComboBox *operation = window.findChild<QComboBox *>("l3op");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(operation);
        QVERIFY(button);
        QVERIFY(label);

        QTest::keyClicks(lineEdit1, "1/2");
        QTest::keyClicks(lineEdit2, "something");
        operation->setCurrentIndex(0); // Сложение — вторая дробь обязательна

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(),
                 QString("Результат: Ошибка: Введите вторую дробь в формате 'числитель/знаменатель'."));
    }

    void testWrongAnswer_DivisionByZero()
    {
        MainWindow window;
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        QLineEdit *lineEdit1 = window.findChild<QLineEdit *>("l3i1");
        QLineEdit *lineEdit2 = window.findChild<QLineEdit *>("l3i2");
        QComboBox *operation = window.findChild<QComboBox *>("l3op");
        QPushButton *button = window.findChild<QPushButton *>("l3b");
        QLabel *label = window.findChild<QLabel *>("l3r");

        QVERIFY(lineEdit1);
        QVERIFY(lineEdit2);
        QVERIFY(operation);
        QVERIFY(button);
        QVERIFY(label);

        QTest::keyClicks(lineEdit1, "1/2");
        QTest::keyClicks(lineEdit2, "0/5"); // числитель 0 => дробь равна нулю
        operation->setCurrentIndex(3);      // Деление

        QTest::mouseClick(button, Qt::LeftButton);

        QCOMPARE(label->text(), QString("Результат: Ошибка: деление на ноль."));
    }
};

QTEST_MAIN(Testlab3)
#include "test_lab3.moc"