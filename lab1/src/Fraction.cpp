#include "Fraction.h"

Fraction::Fraction(int num, int denom) : numerator(num), denominator(denom)
{
    fractionConversion();
}

Fraction::Fraction(const Fraction &other) : numerator(other.numerator),
                                            denominator(other.denominator)
{
}

Fraction::Fraction(Fraction &&other) noexcept : numerator(other.numerator),
                                                denominator(other.denominator)
{
    other.numerator = 0;
    other.denominator = 1;
}

Fraction::~Fraction()
{
}

void Fraction::fractionConversion()
{
    if (denominator == 0)
    {
        denominator = 1;
    }
    if (denominator > abs(numerator))
    {
        numerator %= denominator;
    }
}

unsigned int denomNOD(unsigned int a, unsigned int b)
{
    unsigned int m = a > b ? a : b;
    unsigned int n = a < b ? a : b;
    while (n > 0)
    {
        int temp = n;
        n = m % n;
        m = temp;
    }
    return m;
}

unsigned int denomNOK(unsigned int a, unsigned int b)
{
    unsigned int c = denomNOD(a, b);
    return (a * b) / c;
}

void bringToSameDenom(Fraction &f1, Fraction &f2)
{
    unsigned int NOK = denomNOK(f1.denominator, f2.denominator);
    f1.numerator *= NOK / f1.denominator;
    f1.denominator = NOK;
    f2.numerator *= NOK / f2.denominator;
    f2.denominator = NOK;
}

double fracToDouble(Fraction &frac)
{
    return static_cast<double>(frac.numerator) / frac.denominator;
}

Fraction operationPerform(Fraction a, Fraction b, int operators)
{
    switch (operators)
    {
    case 1:
        bringToSameDenom(a, b);
        a.numerator += b.numerator;
        break;
    case 2:
        bringToSameDenom(a, b);
        a.numerator -= b.numerator;
        break;
    case 3:
        a.numerator *= b.numerator;
        a.denominator *= b.denominator;
        break;
    case 4:
        a.numerator *= b.denominator;
        a.denominator *= b.numerator;
        break;
    }
    a.fractionConversion();
    return a;
}

Fraction Fraction::operator+(const Fraction &num) const
{
    Fraction temp(*this);
    temp = operationPerform(temp, num, 1);
    return temp;
}

Fraction Fraction::operator-(const Fraction &num) const
{
    Fraction temp(*this);
    temp = operationPerform(temp, num, 2);
    return temp;
}

Fraction Fraction::operator*(const Fraction &num) const
{
    Fraction temp(*this);
    temp = operationPerform(temp, num, 3);
    return temp;
}

Fraction Fraction::operator/(const Fraction &num) const
{
    Fraction temp(*this);
    temp = operationPerform(temp, num, 4);
    return temp;
}

Fraction Fraction::operator+(const int &num) const
{
    Fraction temp(*this);
    temp.numerator += denominator * num;
    return temp;
}

Fraction Fraction::operator-(const int &num) const
{
    Fraction temp(*this);
    temp.numerator -= denominator * num;
    return temp;
}

Fraction Fraction::operator*(const int &num) const
{
    Fraction temp(*this);
    temp.numerator *= denominator * num;
    return temp;
}

Fraction Fraction::operator/(const int &num) const
{
    Fraction temp(*this);
    temp.numerator /= denominator * num;
    return temp;
}

double Fraction::operator+(const double &num) const
{
    Fraction temp(*this);
    return fracToDouble(temp) + num;
}

double Fraction::operator-(const double &num) const
{
    Fraction temp(*this);
    return fracToDouble(temp) - num;
}

double Fraction::operator*(const double &num) const
{
    Fraction temp(*this);
    return fracToDouble(temp) * num;
}

double Fraction::operator/(const double &num) const
{
    Fraction temp(*this);
    return fracToDouble(temp) / num;
}

Fraction &Fraction::operator+=(const Fraction &other)
{
    Fraction temp(*this);
    *this = operationPerform(temp, other, 1);
    this->fractionConversion();
    return *this;
}

Fraction &Fraction::operator-=(const Fraction &other)
{
    Fraction temp(*this);
    *this = operationPerform(temp, other, 2);
    this->fractionConversion();
    return *this;
}

Fraction &Fraction::operator*=(const Fraction &other)
{
    Fraction temp(*this);
    *this = operationPerform(temp, other, 3);
    this->fractionConversion();
    return *this;
}

Fraction &Fraction::operator/=(const Fraction &other)
{
    Fraction temp(*this);
    *this = operationPerform(temp, other, 4);
    this->fractionConversion();
    return *this;
}

Fraction &Fraction::operator=(const Fraction &other)
{
    numerator = other.numerator;
    denominator = other.denominator;
    return *this;
}

Fraction Fraction::operator!()
{
    Fraction temp(*this);
    temp.numerator = denominator;
    temp.denominator = numerator;
    return temp;
}

Fraction Fraction::operator-()
{
    numerator *= -1;
    return *this;
}

Fraction &Fraction::operator++()
{
    numerator += static_cast<int>(denominator);
    return *this;
}

Fraction &Fraction::operator--()
{
    numerator -= static_cast<int>(denominator);
    return *this;
}

Fraction Fraction::operator++(int)
{
    Fraction temp(*this);
    numerator += static_cast<int>(denominator);
    return temp;
}

Fraction Fraction::operator--(int)
{
    Fraction temp(*this);
    numerator -= static_cast<int>(denominator);
    return temp;
}

bool boolOperationPerform(Fraction a, Fraction b, int bool_operators)
{
    bringToSameDenom(a, b);
    switch (bool_operators)
    {
    case 1:
        return a.numerator > b.numerator;
        break;
    case 2:
        return a.numerator < b.numerator;
        break;
    case 3:
        return a.numerator >= b.numerator;
        break;
    case 4:
        return a.numerator <= b.numerator;
        break;
    case 5:
        return a.numerator == b.numerator;
        break;
    case 6:
        return a.numerator != b.numerator;
        break;
    default:
        return false;
    }
}

bool Fraction::operator>(const Fraction &other) const
{
    Fraction temp(*this);
    return boolOperationPerform(temp, other, 1);
}

bool Fraction::operator<(const Fraction &other) const
{
    Fraction temp(*this);
    return boolOperationPerform(temp, other, 2);
}

bool Fraction::operator>=(const Fraction &other) const
{
    Fraction temp(*this);
    return boolOperationPerform(temp, other, 3);
}

bool Fraction::operator<=(const Fraction &other) const
{
    Fraction temp(*this);
    return boolOperationPerform(temp, other, 4);
}

bool Fraction::operator==(const Fraction &other) const
{
    Fraction temp(*this);
    return boolOperationPerform(temp, other, 5);
}

bool Fraction::operator!=(const Fraction &other) const
{
    Fraction temp(*this);
    return boolOperationPerform(temp, other, 6);
}

std::istream &operator>>(std::istream &in, Fraction &FracToWrite)
{
    char slash;

    in >> FracToWrite.numerator;
    in >> slash;
    in >> FracToWrite.denominator;
    return in;
}

std::ostream &operator<<(std::ostream &out, const Fraction &FracToPrint)
{
    if (FracToPrint.numerator != 0)
        out << FracToPrint.numerator << "/" << FracToPrint.denominator;
    else
    {
        out << '0';
    }
    return out;
}

QTextStream &operator<<(QTextStream &out, const Fraction &FracToPrint)
{
    if (FracToPrint.numerator != 0)
        out << FracToPrint.numerator << "/" << FracToPrint.denominator;
    else
    {
        out << '0';
    }
    return out;
}