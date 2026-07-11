#ifndef BALL_H
#define BALL_H

#include "GameObject.h"

class Ball : public GameObject
{
public:
    Ball() = default;
    Ball(Vector2D position, double radius, Vector2D velocity)
        : GameObject(position, Vector2D(radius * 2.0, radius * 2.0))
    {
        m_velocity = velocity;
    }

    double radius() const { return m_size.x / 2.0; }

    void reflectX() { m_velocity.x = -m_velocity.x; }
    void reflectY() { m_velocity.y = -m_velocity.y; }

    bool isStuckToPaddle() const { return m_stuck; }
    void setStuckToPaddle(bool stuck) { m_stuck = stuck; }

private:
    bool m_stuck = true;
};

#endif // BALL_H
