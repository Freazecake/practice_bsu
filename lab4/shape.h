#ifndef SHAPE_H
#define SHAPE_H

#include <QColor>
#include <QVector>
#include <QPointF>
#include <QRectF>
#include <QDataStream>

class QPainter;

enum class ShapeType : quint8
{
    Line = 0,
    Rectangle = 1,
    Ellipse = 2,
    Circle = 3,
    Polygon = 4,
    Freehand = 5
};

class Shape
{
public:
    explicit Shape(ShapeType type);
    virtual ~Shape() = default;

    virtual void draw(QPainter &painter) const = 0;
    virtual bool hitTest(const QPointF &pos) const = 0;
    virtual QRectF boundingRect() const = 0;
    virtual Shape *clone() const = 0;

    virtual QVector<QPointF> resizeHandles() const { return {}; }
    virtual int pointIndexForHandle(int handleIndex) const { return handleIndex; }

    virtual void moveBy(const QPointF &delta);

    ShapeType type() const { return m_type; }
    bool isFilled() const { return m_fillColor.alpha() > 0; }

    QColor strokeColor() const { return m_strokeColor; }
    void setStrokeColor(const QColor &c) { m_strokeColor = c; }

    QColor fillColor() const { return m_fillColor; }
    void setFillColor(const QColor &c) { m_fillColor = c; }

    int lineWidth() const { return m_lineWidth; }
    void setLineWidth(int w) { m_lineWidth = w; }

    bool isSelected() const { return m_selected; }
    void setSelected(bool s) { m_selected = s; }

    const QVector<QPointF> &points() const { return m_points; }
    QVector<QPointF> &points() { return m_points; }

    void write(QDataStream &out) const;
    static Shape *read(QDataStream &in);

    static const char *typeName(ShapeType t);
    static ShapeType typeFromName(const QString &name);

protected:
    ShapeType m_type;
    QColor m_strokeColor = Qt::black;
    QColor m_fillColor = QColor(0, 0, 0, 0);
    int m_lineWidth = 2;
    bool m_selected = false;
    QVector<QPointF> m_points;

    static qreal distanceToSegment(const QPointF &p, const QPointF &a, const QPointF &b);
};

#endif // SHAPE_H
