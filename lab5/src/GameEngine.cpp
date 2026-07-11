#include "GameEngine.h"
#include <QSettings>
#include <algorithm>
#include <cmath>

namespace
{
    constexpr double kPaddleSpeed = 480.0;
    constexpr double kBallSpeed = 320.0;
    constexpr int kBrickRows = 5;
    constexpr int kBrickCols = 8;
    constexpr double kPowerUpChance = 0.18;
}

GameEngine::GameEngine(double fieldWidth, double fieldHeight)
    : m_fieldWidth(fieldWidth), m_fieldHeight(fieldHeight)
{
    m_paddle = Paddle(Vector2D(fieldWidth / 2.0, fieldHeight - 30.0), Vector2D(100.0, 16.0));
    loadHighScore();
}

void GameEngine::startNewGame()
{
    m_score = 0;
    m_lives = 3;
    m_level = 1;
    m_elapsedTime = 0.0;
    m_paddle.setSize(Vector2D(m_paddle.baseWidth(), 16.0));
    m_paddle.setPosition(Vector2D(m_fieldWidth / 2.0, m_fieldHeight - 30.0));
    m_powerUps.clear();
    loadLevel(m_level);
    resetBallOnPaddle();
    m_state = GameState::Playing;
}

void GameEngine::startNextLevel()
{
    m_powerUps.clear();
    loadLevel(m_level);
    resetBallOnPaddle();
    m_state = GameState::Playing;
}

void GameEngine::togglePause()
{
    if (m_state == GameState::Playing)
    {
        m_state = GameState::Paused;
    }
    else if (m_state == GameState::Paused)
    {
        m_state = GameState::Playing;
    }
}

void GameEngine::loadLevel(int level)
{
    m_bricks.clear();
    m_balls.clear();

    double marginTop = 60.0;
    double marginSide = 40.0;
    double gap = 6.0;
    double areaWidth = m_fieldWidth - 2 * marginSide;
    double brickW = (areaWidth - (kBrickCols - 1) * gap) / kBrickCols;
    double brickH = 24.0;

    for (int row = 0; row < kBrickRows; ++row)
    {
        for (int col = 0; col < kBrickCols; ++col)
        {
            double x = marginSide + brickW / 2.0 + col * (brickW + gap);
            double y = marginTop + brickH / 2.0 + row * (brickH + gap);

            BrickType type = BrickType::Normal;
            int roll = (row * kBrickCols + col + level) % 7;
            if (roll == 0 && level >= 2)
                type = BrickType::Strong;
            if (roll == 1 && level >= 3)
                type = BrickType::Unbreakable;
            if (roll == 2 && level >= 2)
                type = BrickType::Moving;

            Brick brick(Vector2D(x, y), Vector2D(brickW, brickH), type);
            if (type == BrickType::Moving)
            {
                brick.setVelocity(Vector2D(60.0 + 10.0 * level, 0.0));
                brick.setMoveRange(marginSide + brickW / 2.0,
                                   m_fieldWidth - marginSide - brickW / 2.0);
            }
            m_bricks.push_back(brick);
        }
    }
}

void GameEngine::resetBallOnPaddle()
{
    m_balls.clear();
    Ball ball(Vector2D(m_paddle.position().x, m_paddle.position().y - 20.0),
              8.0, Vector2D(0.0, 0.0));
    ball.setStuckToPaddle(true);
    m_balls.push_back(ball);
}

void GameEngine::movePaddleBy(double dx)
{
    if (m_state != GameState::Playing)
        return;
    Vector2D p = m_paddle.position();
    p.x += dx;
    m_paddle.setPosition(p);
    m_paddle.clampToField(m_fieldWidth);
    m_paddleTargetX = -1.0;
}

void GameEngine::setPaddleTargetX(double x)
{
    if (m_state != GameState::Playing)
        return;
    m_paddleTargetX = x;
}

void GameEngine::launchBall()
{
    if (m_state != GameState::Playing)
        return;
    for (auto &ball : m_balls)
    {
        if (ball.isStuckToPaddle())
        {
            ball.setStuckToPaddle(false);
            ball.setVelocity(Vector2D(kBallSpeed * 0.35, -kBallSpeed));
        }
    }
}

int GameEngine::activeBreakableBricks() const
{
    int count = 0;
    for (const auto &b : m_bricks)
    {
        if (b.isActive() && b.type() != BrickType::Unbreakable)
            count++;
    }
    return count;
}

void GameEngine::update(double dt)
{
    if (m_state != GameState::Playing)
        return;

    m_elapsedTime += dt;

    if (m_paddleTargetX >= 0.0)
    {
        Vector2D p = m_paddle.position();
        double diff = m_paddleTargetX - p.x;
        double maxStep = kPaddleSpeed * dt * 1.5;
        if (std::abs(diff) <= maxStep)
        {
            p.x = m_paddleTargetX;
        }
        else
        {
            p.x += (diff > 0 ? maxStep : -maxStep);
        }
        m_paddle.setPosition(p);
        m_paddle.clampToField(m_fieldWidth);
    }

    for (auto &ball : m_balls)
    {
        if (ball.isStuckToPaddle())
        {
            Vector2D p = ball.position();
            p.x = m_paddle.position().x;
            ball.setPosition(p);
        }
    }

    for (auto &brick : m_bricks)
    {
        if (brick.isActive())
            brick.update(dt);
    }
    for (auto &ball : m_balls)
    {
        if (!ball.isStuckToPaddle())
            ball.update(dt);
    }
    for (auto &pu : m_powerUps)
    {
        pu.update(dt);
        if (pu.position().y - pu.size().y / 2.0 > m_fieldHeight)
        {
            pu.setActive(false);
        }
    }

    handleCollisions(dt);
    removeInactiveObjects();

    if (activeBreakableBricks() == 0)
    {
        m_state = GameState::LevelComplete;
        m_level++;
        saveHighScoreIfNeeded();
    }

    bool anyBallAlive = false;
    for (const auto &ball : m_balls)
    {
        if (ball.isActive())
            anyBallAlive = true;
    }
    if (!anyBallAlive)
    {
        m_lives--;
        if (m_lives <= 0)
        {
            m_state = GameState::GameOver;
            saveHighScoreIfNeeded();
        }
        else
        {
            resetBallOnPaddle();
        }
    }
}

void GameEngine::handleCollisions(double dt)
{
    Q_UNUSED(dt);
    QRectF field(0, 0, m_fieldWidth, m_fieldHeight);

    for (auto &ball : m_balls)
    {
        if (!ball.isActive() || ball.isStuckToPaddle())
            continue;
        QRectF r = ball.rect();

        if (r.left() <= 0)
        {
            Vector2D p = ball.position();
            p.x = ball.radius();
            ball.setPosition(p);
            ball.reflectX();
        }
        else if (r.right() >= m_fieldWidth)
        {
            Vector2D p = ball.position();
            p.x = m_fieldWidth - ball.radius();
            ball.setPosition(p);
            ball.reflectX();
        }
        if (r.top() <= 0)
        {
            Vector2D p = ball.position();
            p.y = ball.radius();
            ball.setPosition(p);
            ball.reflectY();
        }
        else if (r.bottom() >= m_fieldHeight)
        {
            ball.setActive(false);
            continue;
        }

        QRectF paddleRect = m_paddle.rect();
        if (ball.rect().intersects(paddleRect) && ball.velocity().y > 0)
        {
            double hitPos = (ball.position().x - m_paddle.position().x) /
                            (m_paddle.size().x / 2.0);
            hitPos = std::clamp(hitPos, -1.0, 1.0);
            double speed = ball.velocity().length();
            Vector2D newVel(hitPos * speed, -std::abs(speed * 0.9));
            if (newVel.length() < 1.0)
                newVel = Vector2D(0, -speed);
            ball.setVelocity(newVel.normalized() * speed);
            Vector2D p = ball.position();
            p.y = paddleRect.top() - ball.radius() - 0.5;
            ball.setPosition(p);
        }

        for (auto &brick : m_bricks)
        {
            if (!brick.isActive())
                continue;
            QRectF brickRect = brick.rect();
            if (!ball.rect().intersects(brickRect))
                continue;

            QRectF br = ball.rect();
            double overlapLeft = br.right() - brickRect.left();
            double overlapRight = brickRect.right() - br.left();
            double overlapTop = br.bottom() - brickRect.top();
            double overlapBottom = brickRect.bottom() - br.top();

            double minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
            if (minOverlap == overlapLeft || minOverlap == overlapRight)
            {
                ball.reflectX();
            }
            else
            {
                ball.reflectY();
            }

            bool destroyed = brick.hit();
            if (destroyed)
            {
                m_score += brick.scoreValue();
                spawnPowerUpMaybe(brick.position());
            }
            break;
        }
    }

    for (auto &pu : m_powerUps)
    {
        if (!pu.isActive())
            continue;
        if (pu.rect().intersects(m_paddle.rect()))
        {
            applyPowerUp(pu.type());
            pu.setActive(false);
        }
    }
}

void GameEngine::spawnPowerUpMaybe(const Vector2D &at)
{
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    if (chance(m_rng) > kPowerUpChance)
        return;

    std::uniform_int_distribution<int> typeDist(0, 3);
    PowerUpType type = static_cast<PowerUpType>(typeDist(m_rng));
    m_powerUps.push_back(PowerUp(at, type));
}

void GameEngine::applyPowerUp(PowerUpType type)
{
    switch (type)
    {
    case PowerUpType::WidenPaddle:
    {
        Vector2D s = m_paddle.size();
        s.x = std::min(s.x * 1.4, 220.0);
        m_paddle.setSize(s);
        break;
    }
    case PowerUpType::ExtraLife:
        m_lives++;
        break;
    case PowerUpType::SlowBall:
        for (auto &ball : m_balls)
        {
            ball.setVelocity(ball.velocity() * 0.8);
        }
        break;
    case PowerUpType::MultiBall:
        if (!m_balls.isEmpty())
        {
            Ball extra = m_balls.first();
            if (!extra.isStuckToPaddle())
            {
                Vector2D v = extra.velocity();
                extra.setVelocity(Vector2D(-v.x, v.y));
                m_balls.push_back(extra);
            }
        }
        break;
    }
}

void GameEngine::removeInactiveObjects()
{
    m_bricks.erase(std::remove_if(m_bricks.begin(), m_bricks.end(),
                                  [](const Brick &b)
                                  { return !b.isActive(); }),
                   m_bricks.end());
    m_balls.erase(std::remove_if(m_balls.begin(), m_balls.end(),
                                 [](const Ball &b)
                                 { return !b.isActive(); }),
                  m_balls.end());
    m_powerUps.erase(std::remove_if(m_powerUps.begin(), m_powerUps.end(),
                                    [](const PowerUp &p)
                                    { return !p.isActive(); }),
                     m_powerUps.end());
}

void GameEngine::loadHighScore()
{
    QSettings settings("QtLabs", "Arkanoid");
    m_highScore = settings.value("highScore", 0).toInt();
}

void GameEngine::saveHighScoreIfNeeded()
{
    if (m_score > m_highScore)
    {
        m_highScore = m_score;
        QSettings settings("QtLabs", "Arkanoid");
        settings.setValue("highScore", m_highScore);
    }
}
