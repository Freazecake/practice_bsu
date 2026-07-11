#include "GameWidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFont>

namespace
{
    constexpr int kTickMs = 16;
}

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent), m_engine(800.0, 600.0)
{
    setFixedSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    connect(&m_timer, &QTimer::timeout, this, &GameWidget::onTick);
    m_timer.start(kTickMs);
    m_clock.start();
}

void GameWidget::startNewGame()
{
    m_engine.startNewGame();
    emit stateChanged(m_engine.state());
    setFocus();
}

void GameWidget::togglePause()
{
    m_engine.togglePause();
    emit stateChanged(m_engine.state());
}

void GameWidget::onTick()
{
    double dt = m_clock.restart() / 1000.0;
    dt = std::min(dt, 0.05);

    GameState before = m_engine.state();
    if (m_movingLeft)
        m_engine.movePaddleBy(-480.0 * dt);
    if (m_movingRight)
        m_engine.movePaddleBy(480.0 * dt);

    m_engine.update(dt);

    if (m_engine.state() != before)
    {
        emit stateChanged(m_engine.state());
    }
    update();
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(15, 15, 30));

    switch (m_engine.state())
    {
    case GameState::MainMenu:
        drawMenuOverlay(p, "АРКАНОИД",
                        "Enter — начать игру   |   Стрелки/мышь — платформа");
        break;
    case GameState::Playing:
    case GameState::Paused:
        drawScene(p);
        drawHud(p);
        if (m_engine.state() == GameState::Paused)
        {
            drawMenuOverlay(p, "ПАУЗА", "P — продолжить");
        }
        break;
    case GameState::LevelComplete:
        drawScene(p);
        drawHud(p);
        drawMenuOverlay(p, "УРОВЕНЬ ПРОЙДЕН!", "Enter — следующий уровень");
        break;
    case GameState::GameOver:
        drawScene(p);
        drawHud(p);
        drawMenuOverlay(p, "ИГРА ОКОНЧЕНА",
                        QString("Счёт: %1   Рекорд: %2   |   N — новая игра")
                            .arg(m_engine.score())
                            .arg(m_engine.highScore()));
        break;
    }
}

void GameWidget::drawScene(QPainter &p)
{
    // Платформа.
    p.setBrush(QColor(90, 180, 250));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(m_engine.paddle().rect(), 4, 4);

    // Блоки.
    for (const auto &brick : m_engine.bricks())
    {
        QColor color;
        switch (brick.type())
        {
        case BrickType::Normal:
            color = QColor(240, 120, 90);
            break;
        case BrickType::Strong:
            color = QColor(240, 200, 60);
            break;
        case BrickType::Unbreakable:
            color = QColor(120, 120, 130);
            break;
        case BrickType::Moving:
            color = QColor(160, 90, 240);
            break;
        }
        p.setBrush(color);
        p.drawRoundedRect(brick.rect(), 3, 3);
    }

    for (const auto &pu : m_engine.powerUps())
    {
        QColor c;
        switch (pu.type())
        {
        case PowerUpType::WidenPaddle:
            c = QColor(90, 220, 140);
            break;
        case PowerUpType::ExtraLife:
            c = QColor(250, 90, 140);
            break;
        case PowerUpType::SlowBall:
            c = QColor(90, 200, 250);
            break;
        case PowerUpType::MultiBall:
            c = QColor(250, 200, 90);
            break;
        }
        p.setBrush(c);
        p.drawEllipse(pu.rect());
    }

    p.setBrush(Qt::white);
    for (const auto &ball : m_engine.balls())
    {
        p.drawEllipse(ball.rect());
    }
}

void GameWidget::drawHud(QPainter &p)
{
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPointSize(11);
    p.setFont(f);
    QString hud = QString("Счёт: %1    Жизни: %2    Уровень: %3    Время: %4с    Рекорд: %5")
                      .arg(m_engine.score())
                      .arg(m_engine.lives())
                      .arg(m_engine.level())
                      .arg(int(m_engine.elapsedTime()))
                      .arg(m_engine.highScore());
    p.drawText(QRectF(10, 8, width() - 20, 24), Qt::AlignLeft, hud);
}

void GameWidget::drawMenuOverlay(QPainter &p, const QString &title, const QString &subtitle)
{
    p.fillRect(rect(), QColor(0, 0, 0, 150));
    p.setPen(Qt::white);

    QFont titleFont = p.font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.drawText(rect().adjusted(0, -40, 0, -40), Qt::AlignCenter, title);

    QFont subFont = p.font();
    subFont.setPointSize(12);
    subFont.setBold(false);
    p.setFont(subFont);
    p.drawText(rect().adjusted(0, 30, 0, 30), Qt::AlignCenter, subtitle);
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_A:
        m_movingLeft = true;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        m_movingRight = true;
        break;
    case Qt::Key_Space:
        m_engine.launchBall();
        break;
    case Qt::Key_P:
        togglePause();
        break;
    case Qt::Key_N:
        startNewGame();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_engine.state() == GameState::MainMenu)
        {
            startNewGame();
        }
        else if (m_engine.state() == GameState::LevelComplete)
        {
            m_engine.startNextLevel();
            emit stateChanged(m_engine.state());
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_A:
        m_movingLeft = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        m_movingRight = false;
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

void GameWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_engine.setPaddleTargetX(event->localPos().x());
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_engine.state() == GameState::MainMenu)
    {
        startNewGame();
    }
    else if (m_engine.state() == GameState::Playing)
    {
        m_engine.launchBall();
    }
}
