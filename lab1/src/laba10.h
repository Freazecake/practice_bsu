#pragma once

#include "Fraction.h"

#include <iomanip>
#include <QString>
#include <QStringList>
#include <QTextStream>

QString Laba10(QString input1, QString input2, int operation)
{
    QStringList parts1 = input1.split('/');
    if (parts1.size() != 2)
    {
        return "Ошибка: Введите первую дробь в формате 'числитель/знаменатель'.";
    }

    bool ok1n = false, ok1d = false;
    int num1 = parts1[0].toInt(&ok1n);
    int den1 = parts1[1].toInt(&ok1d);
    if (!ok1n || !ok1d)
    {
        return "Ошибка: Числитель и знаменатель первой дроби должны быть целыми числами.";
    }

    Fraction f1{num1, den1};

    QString result;
    QTextStream out(&result);

    // Унарный минус — вторая дробь не требуется
    if (operation == 4)
    {
        Fraction f1_copy(f1);
        out << "-(" << f1 << ") = " << -f1_copy;
        return result;
    }

    QStringList parts2 = input2.split('/');
    if (parts2.size() != 2)
    {
        return "Ошибка: Введите вторую дробь в формате 'числитель/знаменатель'.";
    }

    bool ok2n = false, ok2d = false;
    int num2 = parts2[0].toInt(&ok2n);
    int den2 = parts2[1].toInt(&ok2d);
    if (!ok2n || !ok2d)
    {
        return "Ошибка: Числитель и знаменатель второй дроби должны быть целыми числами.";
    }

    Fraction f2{num2, den2};

    switch (operation)
    {
    case 0:
        out << f1 << " + " << f2 << " = " << (f1 + f2);
        break;
    case 1:
        out << f1 << " - " << f2 << " = " << (f1 - f2);
        break;
    case 2:
        out << f1 << " * " << f2 << " = " << (f1 * f2);
        break;
    case 3:
        if (fracToDouble(f2) == 0.0)
        {
            out << "Ошибка: деление на ноль.";
        }
        else
        {
            out << f1 << " / " << f2 << " = " << (f1 / f2);
        }
        break;
    default:
        out << "Ошибка: неизвестная операция.";
        break;
    }

    return result;
}