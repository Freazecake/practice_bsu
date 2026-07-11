#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

struct Vector2D
{
    double x = 0.0;
    double y = 0.0;

    Vector2D() = default;
    Vector2D(double x_, double y_) : x(x_), y(y_) {}

    Vector2D operator+(const Vector2D &o) const { return {x + o.x, y + o.y}; }
    Vector2D operator-(const Vector2D &o) const { return {x - o.x, y - o.y}; }
    Vector2D operator*(double s) const { return {x * s, y * s}; }

    Vector2D &operator+=(const Vector2D &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }

    double length() const { return std::sqrt(x * x + y * y); }

    Vector2D normalized() const
    {
        double len = length();
        if (len < 1e-9)
            return {0.0, 0.0};
        return {x / len, y / len};
    }
};

#endif // VECTOR2D_H
