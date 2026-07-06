#pragma once

#include "Fraction.h"

#include <iomanip>
#include <QString>
#include <QStringList>
#include <QTextStream>

QString Laba10(QString input1, QString input2)
{
    QStringList parts1 = input1.split('/');
    QStringList parts2 = input2.split('/');

    if (parts1.size() != 2 || parts2.size() != 2)
    {
        return "Ошибка: Введите дроби в формате 'числитель/знаменатель'.";
    }

    QString result;
    QTextStream out(&result);

    Fraction f1{parts1[0].toInt(), parts1[1].toInt()};
    Fraction f2{parts2[0].toInt(), parts2[1].toInt()};

    out << "=== Демонстрация возможностей класса Fraction ===\n\n";

    out << "1. ВХОДНЫЕ ДРОБИ (из параметров):\n";
    out << "   Первая дробь (f1): " << f1 << "\n";
    out << "   Вторая дробь (f2): " << f2 << "\n\n";

    out << "2. КОНСТРУКТОРЫ:\n";
    Fraction f_default;
    out << "   По умолчанию: " << f_default << "\n";
    Fraction f_copy(f1);
    out << "   Конструктор копирования f_copy(f1): " << f_copy << "\n\n";

    out << "3. АРИФМЕТИЧЕСКИЕ ОПЕРАЦИИ:\n";
    Fraction sum = f1 + f2;
    out << "   f1 + f2 = " << sum << "\n";

    Fraction diff = f1 - f2;
    out << "   f1 - f2 = " << diff << "\n";

    Fraction prod = f1 * f2;
    out << "   f1 * f2 = " << prod << "\n";

    Fraction quot = f1 / f2;
    out << "   f1 / f2 = " << quot << "\n\n";

    out << "4. СОСТАВНЫЕ ОПЕРАТОРЫ ПРИСВАИВАНИЯ:\n";
    Fraction c = f1;
    out << "   c = f1 = " << c << "\n";
    c += f2;
    out << "   c += f2: " << c << "\n";
    c -= f2;
    out << "   c -= f2: " << c << "\n\n";

    out << "5. УНАРНЫЕ ОПЕРАТОРЫ:\n";
    Fraction g = f1;
    out << "   g = " << g << "\n";
    out << "   -g (отрицание): " << -g << "\n";

    Fraction prefix = ++g;
    out << "   После ++g: g = " << g << ", результат = " << prefix << "\n";

    Fraction postfix_old = g++;
    out << "   После g++: g = " << g << ", результат (старое значение) = " << postfix_old << "\n\n";

    out << "6. ОПЕРАТОРЫ СРАВНЕНИЯ:\n";
    out << "   f1 == f2: " << (f1 == f2 ? "true" : "false") << "\n";
    out << "   f1 != f2: " << (f1 != f2 ? "true" : "false") << "\n";
    out << "   f1 > f2:  " << (f1 > f2 ? "true" : "false") << "\n";
    out << "   f1 < f2:  " << (f1 < f2 ? "true" : "false") << "\n\n";

    out << "7. ПРЕОБРАЗОВАНИЕ В DOUBLE:\n";
    out << "   f1 в double: " << QString::number(fracToDouble(f1), 'f', 6) << "\n";
    out << "   f2 в double: " << QString::number(fracToDouble(f2), 'f', 6) << "\n\n";

    out << "=== Демонстрация завершена ===";

    return result;
}