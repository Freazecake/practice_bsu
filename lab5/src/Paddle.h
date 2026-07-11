#ifndef PADDLE_H
#define PADDLE_H

#include "GameObject.h"

class Paddle : public GameObject
{
public:
    Paddle() = default;
    Paddle(Vector2D position, Vector2D size) : GameObject(position, size) {}

    double baseWidth() const { return m_baseWidth; }
    void setBaseWidth(double w) { m_baseWidth = w; }

    void clampToField(double fieldWidth)
    {
        double halfW = m_size.x / 2.0;
        if (m_position.x - halfW < 0)
            m_position.x = halfW;
        if (m_position.x + halfW > fieldWidth)
            m_position.x = fieldWidth - halfW;
    }

private:
    double m_baseWidth = 100.0;
};

#endif // PADDLE_H
