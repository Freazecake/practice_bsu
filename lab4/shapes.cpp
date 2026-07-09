#include "shapes.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPolygonF>
#include <QtMath>

static constexpr qreal EDGE_HIT_MARGIN = 6.0;

void LineShape::draw(QPainter &painter) const
{
    if (m_points.size() < 2)
        return;
    QPen pen(m_strokeColor, m_lineWidth);
    painter.setPen(pen);
    painter.drawLine(m_points[0], m_points[1]);
}

bool LineShape::hitTest(const QPointF &pos) const
{
    if (m_points.size() < 2)
        return false;
    return distanceToSegment(pos, m_points[0], m_points[1]) <= (m_lineWidth / 2.0 + EDGE_HIT_MARGIN);
}

QRectF LineShape::boundingRect() const
{
    if (m_points.size() < 2)
        return QRectF();
    return QRectF(m_points[0], m_points[1]).normalized();
}

Shape *LineShape::clone() const
{
    auto *copy = new LineShape(*this);
    return copy;
}

QVector<QPointF> LineShape::resizeHandles() const
{
    if (m_points.size() < 2)
        return {};
    return {m_points[0], m_points[1]};
}

void RectangleShape::draw(QPainter &painter) const
{
    if (m_points.size() < 2)
        return;
    painter.setPen(QPen(m_strokeColor, m_lineWidth));
    painter.setBrush(isFilled() ? QBrush(m_fillColor) : Qt::NoBrush);
    painter.drawRect(QRectF(m_points[0], m_points[1]).normalized());
}

bool RectangleShape::hitTest(const QPointF &pos) const
{
    if (m_points.size() < 2)
        return false;
    QRectF r = QRectF(m_points[0], m_points[1]).normalized();
    if (isFilled())
        return r.adjusted(-EDGE_HIT_MARGIN, -EDGE_HIT_MARGIN, EDGE_HIT_MARGIN, EDGE_HIT_MARGIN).contains(pos);

    QRectF outer = r.adjusted(-EDGE_HIT_MARGIN, -EDGE_HIT_MARGIN, EDGE_HIT_MARGIN, EDGE_HIT_MARGIN);
    QRectF inner = r.adjusted(EDGE_HIT_MARGIN, EDGE_HIT_MARGIN, -EDGE_HIT_MARGIN, -EDGE_HIT_MARGIN);
    return outer.contains(pos) && !inner.contains(pos);
}

QRectF RectangleShape::boundingRect() const
{
    if (m_points.size() < 2)
        return QRectF();
    return QRectF(m_points[0], m_points[1]).normalized();
}

Shape *RectangleShape::clone() const { return new RectangleShape(*this); }

QVector<QPointF> RectangleShape::resizeHandles() const
{
    if (m_points.size() < 2)
        return {};
    return {m_points[1]};
}

void EllipseShape::draw(QPainter &painter) const
{
    if (m_points.size() < 2)
        return;
    painter.setPen(QPen(m_strokeColor, m_lineWidth));
    painter.setBrush(isFilled() ? QBrush(m_fillColor) : Qt::NoBrush);
    painter.drawEllipse(QRectF(m_points[0], m_points[1]).normalized());
}

bool EllipseShape::hitTest(const QPointF &pos) const
{
    if (m_points.size() < 2)
        return false;
    QRectF r = QRectF(m_points[0], m_points[1]).normalized();
    if (r.width() < 1 || r.height() < 1)
        return false;

    QPointF center = r.center();
    qreal rx = r.width() / 2.0;
    qreal ry = r.height() / 2.0;
    qreal dx = (pos.x() - center.x()) / rx;
    qreal dy = (pos.y() - center.y()) / ry;
    qreal value = dx * dx + dy * dy;

    if (isFilled())
        return value <= 1.0;

    qreal margin = EDGE_HIT_MARGIN / qMin(rx, ry);
    return value >= 1.0 - margin && value <= 1.0 + margin;
}

QRectF EllipseShape::boundingRect() const
{
    if (m_points.size() < 2)
        return QRectF();
    return QRectF(m_points[0], m_points[1]).normalized();
}

Shape *EllipseShape::clone() const { return new EllipseShape(*this); }

QVector<QPointF> EllipseShape::resizeHandles() const
{
    if (m_points.size() < 2)
        return {};
    return {m_points[1]};
}

qreal CircleShape::radius() const
{
    if (m_points.size() < 2)
        return 0;
    return QLineF(m_points[0], m_points[1]).length();
}

void CircleShape::draw(QPainter &painter) const
{
    if (m_points.size() < 2)
        return;
    painter.setPen(QPen(m_strokeColor, m_lineWidth));
    painter.setBrush(isFilled() ? QBrush(m_fillColor) : Qt::NoBrush);
    qreal r = radius();
    painter.drawEllipse(m_points[0], r, r);
}

bool CircleShape::hitTest(const QPointF &pos) const
{
    if (m_points.size() < 2)
        return false;
    qreal d = QLineF(pos, m_points[0]).length();
    qreal r = radius();
    if (isFilled())
        return d <= r + EDGE_HIT_MARGIN;
    return qAbs(d - r) <= EDGE_HIT_MARGIN;
}

QRectF CircleShape::boundingRect() const
{
    if (m_points.size() < 2)
        return QRectF();
    qreal r = radius();
    return QRectF(m_points[0] - QPointF(r, r), QSizeF(r * 2, r * 2));
}

Shape *CircleShape::clone() const { return new CircleShape(*this); }

QVector<QPointF> CircleShape::resizeHandles() const
{
    if (m_points.size() < 2)
        return {};
    return {m_points[1]};
}

void PolygonShape::draw(QPainter &painter) const
{
    if (m_points.size() < 2)
        return;
    painter.setPen(QPen(m_strokeColor, m_lineWidth));
    QPolygonF poly(m_points);
    if (isFilled())
    {
        painter.setBrush(QBrush(m_fillColor));
        painter.drawPolygon(poly);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
        painter.drawPolyline(poly);
        painter.drawLine(m_points.last(), m_points.first());
    }
}

bool PolygonShape::hitTest(const QPointF &pos) const
{
    if (m_points.size() < 3)
        return false;
    QPolygonF poly(m_points);
    if (isFilled())
        return poly.containsPoint(pos, Qt::OddEvenFill);

    for (int i = 0; i < m_points.size(); ++i)
    {
        const QPointF &a = m_points[i];
        const QPointF &b = m_points[(i + 1) % m_points.size()];
        if (distanceToSegment(pos, a, b) <= EDGE_HIT_MARGIN)
            return true;
    }
    return false;
}

QRectF PolygonShape::boundingRect() const
{
    return QPolygonF(m_points).boundingRect();
}

Shape *PolygonShape::clone() const { return new PolygonShape(*this); }

void FreehandShape::draw(QPainter &painter) const
{
    if (m_points.size() < 2)
        return;
    painter.setPen(QPen(m_strokeColor, m_lineWidth));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolyline(QPolygonF(m_points));
}

bool FreehandShape::hitTest(const QPointF &pos) const
{
    for (int i = 0; i + 1 < m_points.size(); ++i)
        if (distanceToSegment(pos, m_points[i], m_points[i + 1]) <= (m_lineWidth / 2.0 + EDGE_HIT_MARGIN))
            return true;
    return false;
}

QRectF FreehandShape::boundingRect() const
{
    return QPolygonF(m_points).boundingRect();
}

Shape *FreehandShape::clone() const { return new FreehandShape(*this); }
