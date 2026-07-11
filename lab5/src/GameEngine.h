#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QVector>
#include <QString>
#include <random>
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "PowerUp.h"

enum class GameState
{
    MainMenu,
    Playing,
    Paused,
    LevelComplete,
    GameOver
};

class GameEngine
{
    friend class TestGameEngine;

public:
    explicit GameEngine(double fieldWidth = 800.0, double fieldHeight = 600.0);

    void startNewGame();
    void startNextLevel();
    void togglePause();
    void update(double dt);

    void movePaddleBy(double dx);
    void setPaddleTargetX(double x);
    void launchBall();

    GameState state() const { return m_state; }
    int score() const { return m_score; }
    int lives() const { return m_lives; }
    int level() const { return m_level; }
    double elapsedTime() const { return m_elapsedTime; }
    int highScore() const { return m_highScore; }

    double fieldWidth() const { return m_fieldWidth; }
    double fieldHeight() const { return m_fieldHeight; }

    const Paddle &paddle() const { return m_paddle; }
    const QVector<Ball> &balls() const { return m_balls; }
    const QVector<Brick> &bricks() const { return m_bricks; }
    const QVector<PowerUp> &powerUps() const { return m_powerUps; }

    int activeBreakableBricks() const;

    void loadHighScore();
    void saveHighScoreIfNeeded();

private:
    void loadLevel(int level);
    void resetBallOnPaddle();
    void handleCollisions(double dt);
    void spawnPowerUpMaybe(const Vector2D &at);
    void applyPowerUp(PowerUpType type);
    void removeInactiveObjects();

    double m_fieldWidth;
    double m_fieldHeight;

    GameState m_state = GameState::MainMenu;

    Paddle m_paddle;
    QVector<Ball> m_balls;
    QVector<Brick> m_bricks;
    QVector<PowerUp> m_powerUps;

    int m_score = 0;
    int m_lives = 3;
    int m_level = 1;
    double m_elapsedTime = 0.0;
    int m_highScore = 0;

    double m_paddleTargetX = -1.0;

    std::mt19937 m_rng{std::random_device{}()};
};

#endif // GAMEENGINE_H
