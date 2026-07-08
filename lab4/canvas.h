#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QByteArray>
#include "scene.h"

class Canvas : public QWidget
{
    Q_OBJECT

public:
    enum class Tool { Select, Line, Rectangle, Ellipse, Circle, Polygon, Freehand };

    explicit Canvas(QWidget *parent = nullptr);
    ~Canvas() override;

    void setTool(Tool tool);
    Tool tool() const { return m_tool; }

    void setStrokeColor(const QColor &color);
    void setFillColor(const QColor &color);
    void setNoFill(bool noFill);
    void setLineWidth(int width);

    QColor currentStrokeColor() const { return m_currentStroke; }
    QColor currentFillColor() const { return m_currentFill; }
    int currentLineWidth() const { return m_currentLineWidth; }

    bool hasSelection() const { return m_selectedIndex >= 0; }
    Shape *selectedShape() const;

    bool isModified() const { return m_modified; }
    QString currentFilePath() const { return m_currentFilePath; }
    void setCurrentFilePath(const QString &path) { m_currentFilePath = path; }

    bool canUndo() const { return !m_undoStack.isEmpty(); }
    bool canRedo() const { return !m_redoStack.isEmpty(); }
    bool hasClipboard() const { return !m_clipboard.isEmpty(); }

    Scene &scene() { return m_scene; }
    const Scene &scene() const { return m_scene; }

    bool loadFromFile(const QString &path, QString *errorMessage);
    bool saveToFile(const QString &path, QString *errorMessage) const;
    void markSaved();

public slots:
    void newScene();
    void deleteSelected();
    void clearAll();
    void undo();
    void redo();
    void copySelected();
    void pasteClipboard();
    void bringSelectedToFront();
    void sendSelectedToBack();
    void raiseSelected();
    void lowerSelected();
    void zoomIn();
    void zoomOut();
    void zoomReset();

signals:
    void selectionChanged(Shape *shape);
    void modifiedChanged(bool modified);
    void statusMessage(const QString &text);
    void zoomChanged(qreal zoom);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPointF toScene(const QPoint &widgetPos) const;
    void pushUndoSnapshot();
    void applyModified(bool modified);
    void selectIndex(int index);
    void finishPolygonIfNeeded(bool keepIfValid);
    int handleHitTest(const QPointF &scenePos) const;
    void showContextMenu(const QPoint &widgetPos, const QPointF &scenePos);
    static Shape *createShapeForTool(Tool tool);

    Scene m_scene;
    Tool m_tool = Tool::Select;

    QColor m_currentStroke = Qt::black;
    QColor m_currentFill = QColor(0, 0, 0, 0);
    int m_currentLineWidth = 2;
    qreal m_zoom = 1.0;

    Shape *m_currentDraft = nullptr;
    bool m_isDrawingPolygon = false;
    QPointF m_polygonPreviewPos;

    int m_selectedIndex = -1;
    bool m_dragging = false;
    bool m_resizing = false;
    int m_activeHandle = -1;
    QPointF m_dragLastPos;

    QVector<QByteArray> m_undoStack;
    QVector<QByteArray> m_redoStack;
    QByteArray m_clipboard;

    QString m_currentFilePath;
    bool m_modified = false;

    static constexpr int MAX_UNDO_STEPS = 50;
    static constexpr qreal HANDLE_HIT_RADIUS = 7.0;
    static constexpr qreal HANDLE_DRAW_SIZE = 6.0;
};

#endif // CANVAS_H
