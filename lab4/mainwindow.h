#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canvas.h"

class QComboBox;
class QSpinBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QAction;
class Shape;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onToolChanged(int index);
    void onPickStrokeColor();
    void onPickFillColor();
    void onNoFillToggled(bool checked);
    void onLineWidthChanged(int value);
    void onSelectionChanged(Shape *shape);
    void onModifiedChanged(bool modified);
    void onZoomChanged(qreal zoom);
    void onStatusMessage(const QString &text);

    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();
    void onAbout();

private:
    void setupToolbar();
    void setupMenu();
    void updateColorButtons();
    bool confirmDiscardChangesIfNeeded();
    void updateWindowTitle();

    Canvas *m_canvas;

    QComboBox *m_toolCombo;
    QPushButton *m_strokeColorBtn;
    QPushButton *m_fillColorBtn;
    QCheckBox *m_noFillCheck;
    QSpinBox *m_lineWidthSpin;
    QLabel *m_zoomLabel;

    QColor m_strokeColor = Qt::black;
    QColor m_fillColor = Qt::gray;

    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_pasteAction;
};

#endif // MAINWINDOW_H
