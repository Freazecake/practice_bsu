#include "MainWindow.h"
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_gameWidget = new GameWidget(this);
    setCentralWidget(m_gameWidget);
    setWindowTitle("Qt ЛР №5 — Арканоид");
    setFixedSize(sizeHint());

    buildMenu();
    statusBar()->showMessage("Готово. Enter — начать игру.");

    connect(m_gameWidget, &GameWidget::stateChanged, this, &MainWindow::onStateChanged);
    m_gameWidget->setFocus();
}

void MainWindow::buildMenu()
{
    QMenu *gameMenu = menuBar()->addMenu("&Игра");

    QAction *newGameAction = gameMenu->addAction("&Новая игра\tN");
    connect(newGameAction, &QAction::triggered, m_gameWidget, &GameWidget::startNewGame);

    QAction *pauseAction = gameMenu->addAction("&Пауза\tP");
    connect(pauseAction, &QAction::triggered, m_gameWidget, &GameWidget::togglePause);

    gameMenu->addSeparator();
    QAction *exitAction = gameMenu->addAction("&Выход");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    QMenu *helpMenu = menuBar()->addMenu("&Справка");
    QAction *aboutAction = helpMenu->addAction("&О программе");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::onStateChanged(GameState state)
{
    switch (state)
    {
    case GameState::MainMenu:
        statusBar()->showMessage("Главное меню");
        break;
    case GameState::Playing:
        statusBar()->showMessage("Игра идёт");
        break;
    case GameState::Paused:
        statusBar()->showMessage("Пауза");
        break;
    case GameState::LevelComplete:
        statusBar()->showMessage("Уровень пройден!");
        break;
    case GameState::GameOver:
        statusBar()->showMessage("Игра окончена");
        break;
    }
}

void MainWindow::showAbout()
{
    QMessageBox::information(this, "О программе",
                             "Лаба 5: Арканоид");
}
