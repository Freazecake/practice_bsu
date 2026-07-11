#include <QtTest/QtTest>
#include "GameEngine.h"
#include "Brick.h"
#include "Ball.h"
#include "Paddle.h"

class TestGameEngine : public QObject
{
    Q_OBJECT

private slots:
    void brickHitReducesHitPointsAndDestroys();
    void unbreakableBrickCannotBeDestroyed();
    void ballReflectionInvertsVelocity();
    void paddleClampsToFieldBounds();

    void newGameInitializesExpectedState();
    void atLeastTenObjectsExistSimultaneously();
    void movePaddleByRespectsFieldBounds();
    void launchBallUnsticksBallFromPaddle();
    void pauseTogglesPlayingState();
    void simulationOverTimeProgressesGame();
    void startNextLevelKeepsScoreAndAdvancesLevel();
};

void TestGameEngine::brickHitReducesHitPointsAndDestroys()
{
    Brick strong(Vector2D(0, 0), Vector2D(10, 10), BrickType::Strong);
    QCOMPARE(strong.hitPoints(), 2);

    bool destroyedFirstHit = strong.hit();
    QVERIFY(!destroyedFirstHit);
    QCOMPARE(strong.hitPoints(), 1);
    QVERIFY(strong.isActive());

    bool destroyedSecondHit = strong.hit();
    QVERIFY(destroyedSecondHit);
    QVERIFY(!strong.isActive());
}

void TestGameEngine::unbreakableBrickCannotBeDestroyed()
{
    Brick unbreakable(Vector2D(0, 0), Vector2D(10, 10), BrickType::Unbreakable);
    for (int i = 0; i < 10; ++i)
    {
        bool destroyed = unbreakable.hit();
        QVERIFY(!destroyed);
    }
    QVERIFY(unbreakable.isActive());
}

void TestGameEngine::ballReflectionInvertsVelocity()
{
    Ball ball(Vector2D(100, 100), 8.0, Vector2D(150.0, -200.0));
    ball.reflectX();
    QCOMPARE(ball.velocity().x, -150.0);
    QCOMPARE(ball.velocity().y, -200.0);

    ball.reflectY();
    QCOMPARE(ball.velocity().x, -150.0);
    QCOMPARE(ball.velocity().y, 200.0);
}

void TestGameEngine::paddleClampsToFieldBounds()
{
    Paddle paddle(Vector2D(-50, 500), Vector2D(100, 16));
    paddle.clampToField(800.0);
    QCOMPARE(paddle.position().x, 50.0);

    paddle.setPosition(Vector2D(900, 500));
    paddle.clampToField(800.0);
    QCOMPARE(paddle.position().x, 750.0);
}

void TestGameEngine::newGameInitializesExpectedState()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();

    QCOMPARE(engine.state(), GameState::Playing);
    QCOMPARE(engine.score(), 0);
    QCOMPARE(engine.lives(), 3);
    QCOMPARE(engine.level(), 1);
    QCOMPARE(engine.balls().size(), 1);
    QVERIFY(engine.balls().first().isStuckToPaddle());
    QVERIFY(engine.bricks().size() > 0);
}

void TestGameEngine::atLeastTenObjectsExistSimultaneously()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();
    int totalObjects = engine.bricks().size() + engine.balls().size() + 1;
    QVERIFY(totalObjects >= 10);
}

void TestGameEngine::movePaddleByRespectsFieldBounds()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();

    for (int i = 0; i < 500; ++i)
    {
        engine.movePaddleBy(-10.0);
    }
    QVERIFY(engine.paddle().position().x >= engine.paddle().size().x / 2.0 - 0.001);

    for (int i = 0; i < 500; ++i)
    {
        engine.movePaddleBy(10.0);
    }
    QVERIFY(engine.paddle().position().x <= engine.fieldWidth() - engine.paddle().size().x / 2.0 + 0.001);
}

void TestGameEngine::launchBallUnsticksBallFromPaddle()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();
    QVERIFY(engine.balls().first().isStuckToPaddle());

    engine.launchBall();
    QVERIFY(!engine.balls().first().isStuckToPaddle());
    QVERIFY(engine.balls().first().velocity().length() > 0.0);
}

void TestGameEngine::pauseTogglesPlayingState()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();
    QCOMPARE(engine.state(), GameState::Playing);

    engine.togglePause();
    QCOMPARE(engine.state(), GameState::Paused);

    engine.togglePause();
    QCOMPARE(engine.state(), GameState::Playing);
}

void TestGameEngine::simulationOverTimeProgressesGame()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();
    engine.launchBall();

    int initialBrickCount = engine.bricks().size();

    for (int i = 0; i < 1200 && engine.state() == GameState::Playing; ++i)
    {
        engine.update(1.0 / 60.0);
    }

    bool bricksDestroyedOrScored = (engine.bricks().size() < initialBrickCount) || (engine.score() > 0);
    QVERIFY(bricksDestroyedOrScored);
}

void TestGameEngine::startNextLevelKeepsScoreAndAdvancesLevel()
{
    GameEngine engine(800.0, 600.0);
    engine.startNewGame();

    engine.m_score = 250;
    engine.m_lives = 2;
    for (auto &brick : engine.m_bricks)
    {
        brick.setActive(false);
    }
    engine.m_state = GameState::LevelComplete;
    engine.m_level = 2;

    QCOMPARE(engine.state(), GameState::LevelComplete);

    engine.startNextLevel();

    QCOMPARE(engine.state(), GameState::Playing);
    QCOMPARE(engine.level(), 2);
    QCOMPARE(engine.score(), 250);
    QCOMPARE(engine.lives(), 2);
    QVERIFY(engine.bricks().size() > 0);
    QVERIFY(engine.balls().size() == 1 && engine.balls().first().isStuckToPaddle());
}

QTEST_MAIN(TestGameEngine)
#include "test_gameengine.moc"
