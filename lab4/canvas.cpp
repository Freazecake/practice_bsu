#include "canvas.h"
#include "shapes.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QDataStream>
#include <QIODevice>
#include <QtMath>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(false);
    setMinimumSize(400, 300);
    setAutoFillBackground(false);
}

Canvas::~Canvas()
{
}

Shape *Canvas::createShapeForTool(Tool tool)
{
    switch (tool)
    {
    case Tool::Line: return new LineShape();
    case Tool::Rectangle: return new RectangleShape();
    case Tool::Ellipse: return new EllipseShape();
    case Tool::Circle: return new CircleShape();
    case Tool::Polygon: return new PolygonShape();
    case Tool::Freehand: return new FreehandShape();
    case Tool::Select: return nullptr;
    }
    return nullptr;
}

QPointF Canvas::toScene(const QPoint &widgetPos) const
{
    return QPointF(widgetPos) / m_zoom;
}

void Canvas::setTool(Tool tool)
{
    if (m_tool == Tool::Polygon && m_isDrawingPolygon)
        finishPolygonIfNeeded(true);
    m_tool = tool;
}

void Canvas::setStrokeColor(const QColor &color)
{
    m_currentStroke = color;
    if (m_selectedIndex >= 0)
    {
        pushUndoSnapshot();
        m_scene.at(m_selectedIndex)->setStrokeColor(color);
        applyModified(true);
        update();
    }
}

void Canvas::setFillColor(const QColor &color)
{
    m_currentFill = color;
    if (m_selectedIndex >= 0)
    {
        pushUndoSnapshot();
        m_scene.at(m_selectedIndex)->setFillColor(color);
        applyModified(true);
        update();
    }
}

void Canvas::setNoFill(bool noFill)
{
    QColor c = m_currentFill;
    if (!noFill && c.alpha() == 0)
        c = QColor(Qt::gray);
    setFillColor(noFill ? QColor(0, 0, 0, 0) : c);
}

void Canvas::setLineWidth(int width)
{
    m_currentLineWidth = width;
    if (m_selectedIndex >= 0)
    {
        pushUndoSnapshot();
        m_scene.at(m_selectedIndex)->setLineWidth(width);
        applyModified(true);
        update();
    }
}

Shape *Canvas::selectedShape() const
{
    return m_selectedIndex >= 0 ? m_scene.at(m_selectedIndex) : nullptr;
}

void Canvas::pushUndoSnapshot()
{
    m_undoStack.append(m_scene.toByteArray());
    if (m_undoStack.size() > MAX_UNDO_STEPS)
        m_undoStack.removeFirst();
    m_redoStack.clear();
}

void Canvas::applyModified(bool modified)
{
    if (m_modified != modified)
    {
        m_modified = modified;
        emit modifiedChanged(modified);
    }
}

void Canvas::selectIndex(int index)
{
    if (m_selectedIndex == index)
        return;
    if (m_selectedIndex >= 0 && m_selectedIndex < m_scene.count())
        m_scene.at(m_selectedIndex)->setSelected(false);
    m_selectedIndex = index;
    if (m_selectedIndex >= 0)
        m_scene.at(m_selectedIndex)->setSelected(true);
    emit selectionChanged(selectedShape());
}

void Canvas::finishPolygonIfNeeded(bool keepIfValid)
{
    if (!m_isDrawingPolygon)
        return;
    m_isDrawingPolygon = false;

    int idx = m_scene.count() - 1;
    Shape *draft = m_currentDraft;
    m_currentDraft = nullptr;

    if (!draft)
        return;

    if (!keepIfValid || draft->points().size() < 3)
        m_scene.removeAt(idx);

    update();
}

int Canvas::handleHitTest(const QPointF &scenePos) const
{
    if (m_selectedIndex < 0)
        return -1;
    Shape *sel = m_scene.at(m_selectedIndex);
    QVector<QPointF> handles = sel->resizeHandles();
    for (int i = 0; i < handles.size(); ++i)
        if (QLineF(scenePos, handles.at(i)).length() <= HANDLE_HIT_RADIUS / m_zoom)
            return i;
    return -1;
}

void Canvas::showContextMenu(const QPoint &widgetPos, const QPointF &scenePos)
{
    int hit = m_scene.hitTestTopmost(scenePos);
    if (hit >= 0)
        selectIndex(hit);
    update();

    if (m_selectedIndex < 0)
        return;

    QMenu menu(this);
    QAction *deleteAction = menu.addAction("Удалить");
    menu.addSeparator();
    QAction *frontAction = menu.addAction("На передний план");
    QAction *backAction = menu.addAction("На задний план");
    QAction *raiseAction = menu.addAction("Поднять на слой выше");
    QAction *lowerAction = menu.addAction("Опустить на слой ниже");
    menu.addSeparator();
    QAction *copyAction = menu.addAction("Копировать");

    QAction *chosen = menu.exec(mapToGlobal(widgetPos));
    if (chosen == deleteAction) deleteSelected();
    else if (chosen == frontAction) bringSelectedToFront();
    else if (chosen == backAction) sendSelectedToBack();
    else if (chosen == raiseAction) raiseSelected();
    else if (chosen == lowerAction) lowerSelected();
    else if (chosen == copyAction) copySelected();
}

// ------------------------- События мыши -------------------------

void Canvas::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = toScene(event->pos());

    if (event->button() == Qt::RightButton)
    {
        if (m_isDrawingPolygon)
            finishPolygonIfNeeded(true); // правая кнопка также завершает многоугольник
        else
            showContextMenu(event->pos(), scenePos);
        return;
    }

    if (event->button() != Qt::LeftButton)
        return;

    if (m_tool == Tool::Select)
    {
        int handle = handleHitTest(scenePos);
        if (handle >= 0)
        {
            pushUndoSnapshot();
            m_resizing = true;
            m_activeHandle = handle;
            return;
        }

        int hit = m_scene.hitTestTopmost(scenePos);
        selectIndex(hit);
        if (hit >= 0)
        {
            pushUndoSnapshot();
            m_dragging = true;
            m_dragLastPos = scenePos;
        }
        update();
        return;
    }

    if (m_tool == Tool::Polygon)
    {
        if (!m_isDrawingPolygon)
        {
            pushUndoSnapshot();
            auto *poly = new PolygonShape();
            poly->setStrokeColor(m_currentStroke);
            poly->setFillColor(m_currentFill);
            poly->setLineWidth(m_currentLineWidth);
            poly->points().append(scenePos);
            m_scene.addShape(poly);
            m_currentDraft = poly;
            m_isDrawingPolygon = true;
            selectIndex(-1);
        }
        else if (m_currentDraft)
        {
            m_currentDraft->points().append(scenePos);
        }
        m_polygonPreviewPos = scenePos;
        applyModified(true);
        update();
        return;
    }

    // Line / Rectangle / Ellipse / Circle / Freehand — начало создания фигуры
    pushUndoSnapshot();
    Shape *s = createShapeForTool(m_tool);
    s->setStrokeColor(m_currentStroke);
    s->setFillColor(m_currentFill);
    s->setLineWidth(m_currentLineWidth);
    s->points().append(scenePos);
    s->points().append(scenePos);
    m_scene.addShape(s);
    m_currentDraft = s;
    selectIndex(-1);
    applyModified(true);
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = toScene(event->pos());

    if (m_resizing && m_selectedIndex >= 0)
    {
        Shape *sel = m_scene.at(m_selectedIndex);
        int ptIdx = sel->pointIndexForHandle(m_activeHandle);
        if (ptIdx >= 0 && ptIdx < sel->points().size())
            sel->points()[ptIdx] = scenePos;
        applyModified(true);
        update();
        return;
    }

    if (m_dragging && m_selectedIndex >= 0)
    {
        QPointF delta = scenePos - m_dragLastPos;
        m_scene.at(m_selectedIndex)->moveBy(delta);
        m_dragLastPos = scenePos;
        applyModified(true);
        update();
        return;
    }

    if (m_currentDraft && m_tool == Tool::Freehand)
    {
        m_currentDraft->points().append(scenePos);
        update();
        return;
    }

    if (m_currentDraft && m_tool != Tool::Polygon)
    {
        m_currentDraft->points()[1] = scenePos;
        update();
        return;
    }

    if (m_isDrawingPolygon)
    {
        m_polygonPreviewPos = scenePos;
        update();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (m_resizing)
    {
        m_resizing = false;
        m_activeHandle = -1;
        return;
    }
    if (m_dragging)
    {
        m_dragging = false;
        return;
    }
    if (m_currentDraft && m_tool != Tool::Polygon)
    {
        int idx = m_scene.count() - 1;
        Shape *finished = m_currentDraft;
        m_currentDraft = nullptr;
        if (m_tool != Tool::Freehand &&
            finished->boundingRect().width() < 1 && finished->boundingRect().height() < 1)
        {
            m_scene.removeAt(idx); // фигура нулевого размера (случайный клик) — не сохраняем
        }
        update();
    }
}

void Canvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_isDrawingPolygon)
        finishPolygonIfNeeded(true);
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();
        event->accept();
        return;
    }
    QWidget::wheelEvent(event);
}

// ------------------------- Отрисовка -------------------------

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();
    painter.scale(m_zoom, m_zoom);
    for (Shape *s : m_scene.shapes())
        s->draw(painter);

    if (m_isDrawingPolygon && m_currentDraft && !m_currentDraft->points().isEmpty())
    {
        QPen dashPen(m_currentStroke, 1, Qt::DashLine);
        painter.setPen(dashPen);
        painter.drawLine(m_currentDraft->points().last(), m_polygonPreviewPos);
    }
    painter.restore();

    if (m_selectedIndex >= 0)
    {
        Shape *sel = m_scene.at(m_selectedIndex);
        painter.save();
        painter.scale(m_zoom, m_zoom);

        QPen selPen(QColor(30, 144, 255), 1.0 / m_zoom, Qt::DashLine);
        painter.setPen(selPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(sel->boundingRect().adjusted(-4, -4, 4, 4));

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(30, 144, 255));
        qreal hs = HANDLE_DRAW_SIZE / m_zoom;
        for (const QPointF &h : sel->resizeHandles())
            painter.drawRect(QRectF(h.x() - hs / 2, h.y() - hs / 2, hs, hs));

        painter.restore();
    }
}

// ------------------------- Слоты редактирования -------------------------

void Canvas::newScene()
{
    m_scene.clear();
    m_undoStack.clear();
    m_redoStack.clear();
    m_selectedIndex = -1;
    m_currentFilePath.clear();
    emit selectionChanged(nullptr);
    applyModified(false);
    update();
}

bool Canvas::loadFromFile(const QString &path, QString *errorMessage)
{
    if (!m_scene.loadFromFile(path, errorMessage))
        return false;

    m_undoStack.clear();
    m_redoStack.clear();
    m_selectedIndex = -1;
    m_currentDraft = nullptr;
    m_isDrawingPolygon = false;
    m_currentFilePath = path;
    emit selectionChanged(nullptr);
    applyModified(false);
    update();
    return true;
}

bool Canvas::saveToFile(const QString &path, QString *errorMessage) const
{
    return m_scene.saveToFile(path, errorMessage);
}

void Canvas::markSaved()
{
    applyModified(false);
}

void Canvas::deleteSelected()
{
    if (m_selectedIndex < 0)
        return;
    pushUndoSnapshot();
    m_scene.removeAt(m_selectedIndex);
    m_selectedIndex = -1;
    emit selectionChanged(nullptr);
    applyModified(true);
    update();
}

void Canvas::clearAll()
{
    if (m_scene.count() == 0)
        return;
    pushUndoSnapshot();
    m_scene.clear();
    m_selectedIndex = -1;
    emit selectionChanged(nullptr);
    applyModified(true);
    update();
}

void Canvas::undo()
{
    if (m_undoStack.isEmpty())
        return;
    m_redoStack.append(m_scene.toByteArray());
    QByteArray snapshot = m_undoStack.takeLast();
    m_scene.fromByteArray(snapshot, nullptr);
    m_selectedIndex = -1;
    emit selectionChanged(nullptr);
    applyModified(true);
    update();
}

void Canvas::redo()
{
    if (m_redoStack.isEmpty())
        return;
    m_undoStack.append(m_scene.toByteArray());
    QByteArray snapshot = m_redoStack.takeLast();
    m_scene.fromByteArray(snapshot, nullptr);
    m_selectedIndex = -1;
    emit selectionChanged(nullptr);
    applyModified(true);
    update();
}

void Canvas::copySelected()
{
    if (m_selectedIndex < 0)
        return;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    m_scene.at(m_selectedIndex)->write(out);
    m_clipboard = data;
    emit statusMessage("Объект скопирован");
}

void Canvas::pasteClipboard()
{
    if (m_clipboard.isEmpty())
        return;
    QDataStream in(m_clipboard);
    in.setVersion(QDataStream::Qt_5_15);
    Shape *s = Shape::read(in);
    if (!s)
        return;

    pushUndoSnapshot();
    s->moveBy(QPointF(20, 20));
    s->setSelected(false);
    m_scene.addShape(s);
    selectIndex(m_scene.count() - 1);
    applyModified(true);
    update();
    emit statusMessage("Объект вставлен");
}

void Canvas::bringSelectedToFront()
{
    if (m_selectedIndex < 0)
        return;
    pushUndoSnapshot();
    if (m_scene.bringToFront(m_selectedIndex))
        m_selectedIndex = m_scene.count() - 1;
    applyModified(true);
    update();
}

void Canvas::sendSelectedToBack()
{
    if (m_selectedIndex < 0)
        return;
    pushUndoSnapshot();
    if (m_scene.sendToBack(m_selectedIndex))
        m_selectedIndex = 0;
    applyModified(true);
    update();
}

void Canvas::raiseSelected()
{
    if (m_selectedIndex < 0)
        return;
    pushUndoSnapshot();
    if (m_scene.raise(m_selectedIndex))
        m_selectedIndex += 1;
    applyModified(true);
    update();
}

void Canvas::lowerSelected()
{
    if (m_selectedIndex < 0)
        return;
    pushUndoSnapshot();
    if (m_scene.lower(m_selectedIndex))
        m_selectedIndex -= 1;
    applyModified(true);
    update();
}

void Canvas::zoomIn()
{
    m_zoom = qMin(m_zoom * 1.15, 5.0);
    emit zoomChanged(m_zoom);
    update();
}

void Canvas::zoomOut()
{
    m_zoom = qMax(m_zoom / 1.15, 0.2);
    emit zoomChanged(m_zoom);
    update();
}

void Canvas::zoomReset()
{
    m_zoom = 1.0;
    emit zoomChanged(m_zoom);
    update();
}
