#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include "GameEngine.h"

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);

    GameEngine &engine() { return m_engine; }

public slots:
    void startNewGame();
    void togglePause();

signals:
    void stateChanged(GameState state);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onTick();

private:
    void drawScene(class QPainter &p);
    void drawHud(class QPainter &p);
    void drawMenuOverlay(class QPainter &p, const QString &title, const QString &subtitle);

    GameEngine m_engine;
    QTimer m_timer;
    QElapsedTimer m_clock;

    bool m_movingLeft = false;
    bool m_movingRight = false;
};

#endif // GAMEWIDGET_H
