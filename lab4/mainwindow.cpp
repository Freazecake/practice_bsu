#include "mainwindow.h"
#include "shape.h"

#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_canvas = new Canvas(this);
    setCentralWidget(m_canvas);

    setupToolbar();
    setupMenu();

    connect(m_canvas, &Canvas::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_canvas, &Canvas::modifiedChanged, this, &MainWindow::onModifiedChanged);
    connect(m_canvas, &Canvas::zoomChanged, this, &MainWindow::onZoomChanged);
    connect(m_canvas, &Canvas::statusMessage, this, &MainWindow::onStatusMessage);

    updateColorButtons();
    updateWindowTitle();
    resize(1000, 650);
    statusBar()->showMessage("Готово");
    setWindowTitle("Лаба 4");
}

void MainWindow::setupToolbar()
{
    auto *toolbar = addToolBar("Инструменты");
    toolbar->setMovable(false);

    m_toolCombo = new QComboBox(toolbar);
    m_toolCombo->addItem("Выделение");
    m_toolCombo->addItem("Линия");
    m_toolCombo->addItem("Прямоугольник");
    m_toolCombo->addItem("Эллипс");
    m_toolCombo->addItem("Окружность");
    m_toolCombo->addItem("Многоугольник");
    m_toolCombo->addItem("Кривая (свободно)");
    connect(m_toolCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onToolChanged);
    toolbar->addWidget(new QLabel(" Инструмент: "));
    toolbar->addWidget(m_toolCombo);
    toolbar->addSeparator();

    m_strokeColorBtn = new QPushButton("Контур", toolbar);
    connect(m_strokeColorBtn, &QPushButton::clicked, this, &MainWindow::onPickStrokeColor);
    toolbar->addWidget(m_strokeColorBtn);

    m_fillColorBtn = new QPushButton("Заливка", toolbar);
    connect(m_fillColorBtn, &QPushButton::clicked, this, &MainWindow::onPickFillColor);
    toolbar->addWidget(m_fillColorBtn);

    m_noFillCheck = new QCheckBox("Без заливки", toolbar);
    connect(m_noFillCheck, &QCheckBox::toggled, this, &MainWindow::onNoFillToggled);
    toolbar->addWidget(m_noFillCheck);
    toolbar->addSeparator();

    toolbar->addWidget(new QLabel(" Толщина линии: "));
    m_lineWidthSpin = new QSpinBox(toolbar);
    m_lineWidthSpin->setRange(1, 30);
    m_lineWidthSpin->setValue(2);
    connect(m_lineWidthSpin, &QSpinBox::valueChanged, this, &MainWindow::onLineWidthChanged);
    toolbar->addWidget(m_lineWidthSpin);
    toolbar->addSeparator();

    m_zoomLabel = new QLabel("Масштаб: 100%", toolbar);
    toolbar->addWidget(m_zoomLabel);
}

void MainWindow::setupMenu()
{
    auto *fileMenu = menuBar()->addMenu("&Файл");
    fileMenu->addAction("Новый", QKeySequence::New, this, &MainWindow::onNewFile);
    fileMenu->addSeparator();
    fileMenu->addAction("Открыть...", QKeySequence::Open, this, &MainWindow::onOpenFile);
    fileMenu->addAction("Сохранить", QKeySequence::Save, this, &MainWindow::onSaveFile);
    fileMenu->addAction("Сохранить как...", this, &MainWindow::onSaveFileAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Выход", QKeySequence::Quit, this, &QWidget::close);

    auto *editMenu = menuBar()->addMenu("&Правка");
    m_undoAction = editMenu->addAction("Отменить", QKeySequence::Undo, m_canvas, &Canvas::undo);
    m_redoAction = editMenu->addAction("Повторить", QKeySequence::Redo, m_canvas, &Canvas::redo);
    editMenu->addSeparator();
    editMenu->addAction("Копировать", QKeySequence::Copy, m_canvas, &Canvas::copySelected);
    m_pasteAction = editMenu->addAction("Вставить", QKeySequence::Paste, m_canvas, &Canvas::pasteClipboard);
    editMenu->addAction("Удалить", QKeySequence::Delete, m_canvas, &Canvas::deleteSelected);
    editMenu->addSeparator();
    editMenu->addAction("Очистить холст", m_canvas, &Canvas::clearAll);

    auto *layerMenu = menuBar()->addMenu("&Слои");
    layerMenu->addAction("На передний план", m_canvas, &Canvas::bringSelectedToFront);
    layerMenu->addAction("На задний план", m_canvas, &Canvas::sendSelectedToBack);
    layerMenu->addAction("Поднять на слой выше", m_canvas, &Canvas::raiseSelected);
    layerMenu->addAction("Опустить на слой ниже", m_canvas, &Canvas::lowerSelected);

    auto *viewMenu = menuBar()->addMenu("&Вид");
    viewMenu->addAction("Увеличить", QKeySequence::ZoomIn, m_canvas, &Canvas::zoomIn);
    viewMenu->addAction("Уменьшить", QKeySequence::ZoomOut, m_canvas, &Canvas::zoomOut);
    viewMenu->addAction("Сбросить масштаб", m_canvas, &Canvas::zoomReset);

    auto *helpMenu = menuBar()->addMenu("&Справка");
    helpMenu->addAction("О программе", this, &MainWindow::onAbout);
}

void MainWindow::updateColorButtons()
{
    m_strokeColorBtn->setStyleSheet(QString("background-color: %1;").arg(m_strokeColor.name()));
    m_fillColorBtn->setStyleSheet(m_fillColor.alpha() == 0
                                      ? QString()
                                      : QString("background-color: %1;").arg(m_fillColor.name()));
}

void MainWindow::onToolChanged(int index)
{
    m_canvas->setTool(static_cast<Canvas::Tool>(index));
}

void MainWindow::onPickStrokeColor()
{
    QColor c = QColorDialog::getColor(m_strokeColor, this, "Цвет контура");
    if (!c.isValid())
        return;
    m_strokeColor = c;
    m_canvas->setStrokeColor(c);
    updateColorButtons();
}

void MainWindow::onPickFillColor()
{
    QColor c = QColorDialog::getColor(m_fillColor, this, "Цвет заливки");
    if (!c.isValid())
        return;
    m_fillColor = c;
    if (!m_noFillCheck->isChecked())
        m_canvas->setFillColor(c);
    updateColorButtons();
}

void MainWindow::onNoFillToggled(bool checked)
{
    m_canvas->setNoFill(checked);
    updateColorButtons();
}

void MainWindow::onLineWidthChanged(int value)
{
    m_canvas->setLineWidth(value);
}

void MainWindow::onSelectionChanged(Shape *shape)
{
    m_toolCombo->blockSignals(true);
    m_strokeColorBtn->blockSignals(true);
    m_lineWidthSpin->blockSignals(true);
    m_noFillCheck->blockSignals(true);

    if (shape)
    {
        m_strokeColor = shape->strokeColor();
        m_fillColor = shape->fillColor().alpha() == 0 ? m_fillColor : shape->fillColor();
        m_noFillCheck->setChecked(!shape->isFilled());
        m_lineWidthSpin->setValue(shape->lineWidth());
    }

    updateColorButtons();

    m_toolCombo->blockSignals(false);
    m_strokeColorBtn->blockSignals(false);
    m_lineWidthSpin->blockSignals(false);
    m_noFillCheck->blockSignals(false);
}

void MainWindow::onModifiedChanged(bool modified)
{
    Q_UNUSED(modified);
    updateWindowTitle();
}

void MainWindow::onZoomChanged(qreal zoom)
{
    m_zoomLabel->setText(QString("Масштаб: %1%").arg(qRound(zoom * 100)));
}

void MainWindow::onStatusMessage(const QString &text)
{
    statusBar()->showMessage(text, 3000);
}

void MainWindow::updateWindowTitle()
{
    QString path = m_canvas->currentFilePath();
    QString base = path.isEmpty() ? "новый файл" : path;
    QString title = "Векторный редактор — " + base;
    if (m_canvas->isModified())
        title += " *";
    setWindowTitle(title);
}

bool MainWindow::confirmDiscardChangesIfNeeded()
{
    if (!m_canvas->isModified())
        return true;

    auto reply = QMessageBox::question(
        this, "Несохранённые изменения",
        "Есть несохранённые изменения. Сохранить перед продолжением?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel)
        return false;
    if (reply == QMessageBox::Save)
        onSaveFile();
    return true;
}

void MainWindow::onNewFile()
{
    if (!confirmDiscardChangesIfNeeded())
        return;
    m_canvas->newScene();
    updateWindowTitle();
}

void MainWindow::onOpenFile()
{
    if (!confirmDiscardChangesIfNeeded())
        return;

    QString path = QFileDialog::getOpenFileName(this, "Открыть сцену", QString(), "Файлы векторного редактора (*.vge)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_canvas->loadFromFile(path, &error))
    {
        QMessageBox::critical(this, "Ошибка загрузки", error);
        return;
    }
    updateWindowTitle();
    statusBar()->showMessage("Файл загружен: " + path, 3000);
}

void MainWindow::onSaveFile()
{
    QString path = m_canvas->currentFilePath();
    if (path.isEmpty())
    {
        onSaveFileAs();
        return;
    }
    QString error;
    if (!m_canvas->saveToFile(path, &error))
        QMessageBox::critical(this, "Ошибка сохранения", error);
    else
    {
        m_canvas->markSaved();
        statusBar()->showMessage("Сохранено: " + path, 3000);
    }
}

void MainWindow::onSaveFileAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить сцену как", "scene.vge", "Файлы векторного редактора (*.vge)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_canvas->saveToFile(path, &error))
    {
        QMessageBox::critical(this, "Ошибка сохранения", error);
        return;
    }
    m_canvas->setCurrentFilePath(path);
    m_canvas->markSaved();
    updateWindowTitle();
    statusBar()->showMessage("Сохранено: " + path, 3000);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "О программе",
                       "Лаба 4\n\n"
                       "Фигуры: линия, прямоугольник, эллипс, окружность, многоугольник, кривая.\n"
                       "Возможности: выделение, перемещение, изменение размера, undo/redo,\n"
                       "копирование/вставка, управление слоями, масштабирование, сохранение/загрузка.");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!confirmDiscardChangesIfNeeded())
    {
        event->ignore();
        return;
    }
    event->accept();
}
