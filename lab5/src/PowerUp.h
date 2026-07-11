#ifndef POWERUP_H
#define POWERUP_H

#include "GameObject.h"

enum class PowerUpType
{
    WidenPaddle,
    ExtraLife,
    SlowBall,
    MultiBall
};

class PowerUp : public GameObject
{
public:
    PowerUp() = default;
    PowerUp(Vector2D position, PowerUpType type)
        : GameObject(position, Vector2D(20.0, 20.0)), m_type(type)
    {
        m_velocity = Vector2D(0.0, 120.0);
    }

    PowerUpType type() const { return m_type; }

private:
    PowerUpType m_type = PowerUpType::WidenPaddle;
};

#endif // POWERUP_H
