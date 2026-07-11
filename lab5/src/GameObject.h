#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Vector2D.h"
#include <QRectF>

class GameObject
{
public:
    GameObject() = default;
    GameObject(Vector2D position, Vector2D size)
        : m_position(position), m_size(size) {}
    virtual ~GameObject() = default;

    Vector2D position() const { return m_position; }
    void setPosition(const Vector2D &p) { m_position = p; }

    Vector2D size() const { return m_size; }
    void setSize(const Vector2D &s) { m_size = s; }

    Vector2D velocity() const { return m_velocity; }
    void setVelocity(const Vector2D &v) { m_velocity = v; }

    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    QRectF rect() const
    {
        return QRectF(m_position.x - m_size.x / 2.0,
                      m_position.y - m_size.y / 2.0,
                      m_size.x, m_size.y);
    }

    virtual void update(double dt)
    {
        m_position += m_velocity * dt;
    }

protected:
    Vector2D m_position;
    Vector2D m_size;
    Vector2D m_velocity;
    bool m_active = true;
};

#endif // GAMEOBJECT_H
