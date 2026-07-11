#ifndef BRICK_H
#define BRICK_H

#include "GameObject.h"

enum class BrickType
{
    Normal,
    Strong,
    Unbreakable,
    Moving
};

class Brick : public GameObject
{
public:
    Brick() = default;
    Brick(Vector2D position, Vector2D size, BrickType type)
        : GameObject(position, size), m_type(type)
    {
        switch (type)
        {
        case BrickType::Normal:
            m_hitPoints = 1;
            break;
        case BrickType::Strong:
            m_hitPoints = 2;
            break;
        case BrickType::Unbreakable:
            m_hitPoints = -1;
            break;
        case BrickType::Moving:
            m_hitPoints = 1;
            break;
        }
    }

    BrickType type() const { return m_type; }
    int hitPoints() const { return m_hitPoints; }

    bool hit()
    {
        if (m_type == BrickType::Unbreakable)
            return false;
        m_hitPoints--;
        if (m_hitPoints <= 0)
        {
            m_active = false;
            return true;
        }
        return false;
    }

    int scoreValue() const
    {
        switch (m_type)
        {
        case BrickType::Normal:
            return 10;
        case BrickType::Strong:
            return 25;
        case BrickType::Moving:
            return 20;
        default:
            return 0;
        }
    }

    void update(double dt) override
    {
        GameObject::update(dt);
        if (m_type == BrickType::Moving)
        {
            if (m_position.x <= m_moveMinX || m_position.x >= m_moveMaxX)
            {
                m_velocity.x = -m_velocity.x;
            }
        }
    }

    void setMoveRange(double minX, double maxX)
    {
        m_moveMinX = minX;
        m_moveMaxX = maxX;
    }

private:
    BrickType m_type = BrickType::Normal;
    int m_hitPoints = 1;
    double m_moveMinX = 0.0;
    double m_moveMaxX = 0.0;
};

#endif // BRICK_H
