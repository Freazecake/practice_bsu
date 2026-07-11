#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GameWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onStateChanged(GameState state);
    void showAbout();

private:
    void buildMenu();

    GameWidget *m_gameWidget;
};

#endif // MAINWINDOW_H
