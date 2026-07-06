#pragma once

#include <iostream>
#include <QTextStream>

class Fraction
{
private:
    int numerator;
    int denominator;
    void fractionConversion();
    friend Fraction operationPerform(Fraction, Fraction, int);
    friend bool boolOperationPerform(Fraction, Fraction, int);

public:
    Fraction(int = 0, int = 1);
    Fraction(const Fraction &);
    Fraction(Fraction &&) noexcept;
    ~Fraction();

    friend void bringToSameDenom(Fraction &, Fraction &);
    friend double fracToDouble(Fraction &);

    Fraction operator+(const Fraction &) const;
    Fraction operator-(const Fraction &) const;
    Fraction operator*(const Fraction &) const;
    Fraction operator/(const Fraction &) const;
    Fraction operator+(const int &) const;
    Fraction operator-(const int &) const;
    Fraction operator*(const int &) const;
    Fraction operator/(const int &) const;
    double operator+(const double &) const;
    double operator-(const double &) const;
    double operator*(const double &) const;
    double operator/(const double &) const;

    Fraction &operator+=(const Fraction &);
    Fraction &operator-=(const Fraction &);
    Fraction &operator*=(const Fraction &);
    Fraction &operator/=(const Fraction &);
    Fraction &operator=(const Fraction &);
    Fraction operator!();
    Fraction operator-();
    Fraction &operator--();
    Fraction &operator++();
    Fraction operator--(int);
    Fraction operator++(int);

    bool operator==(const Fraction &) const;
    bool operator>=(const Fraction &) const;
    bool operator<=(const Fraction &) const;
    bool operator>(const Fraction &) const;
    bool operator<(const Fraction &) const;
    bool operator!=(const Fraction &) const;

    friend std::istream &operator>>(std::istream &, Fraction &);
    friend std::ostream &operator<<(std::ostream &, const Fraction &);
    friend QTextStream &operator<<(QTextStream &, const Fraction &);
};