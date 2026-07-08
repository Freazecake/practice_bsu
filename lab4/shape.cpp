#include "shape.h"
#include "shapes.h"

#include <QtMath>
#include <QLineF>

Shape::Shape(ShapeType type)
    : m_type(type)
{
}

void Shape::moveBy(const QPointF &delta)
{
    for (QPointF &p : m_points)
        p += delta;
}

qreal Shape::distanceToSegment(const QPointF &p, const QPointF &a, const QPointF &b)
{
    QPointF ab = b - a;
    qreal lengthSq = QPointF::dotProduct(ab, ab);
    if (lengthSq < 1e-9)
        return QLineF(p, a).length();

    qreal t = QPointF::dotProduct(p - a, ab) / lengthSq;
    t = qBound(0.0, t, 1.0);
    QPointF projection = a + t * ab;
    return QLineF(p, projection).length();
}

const char *Shape::typeName(ShapeType t)
{
    switch (t)
    {
    case ShapeType::Line: return "Line";
    case ShapeType::Rectangle: return "Rectangle";
    case ShapeType::Ellipse: return "Ellipse";
    case ShapeType::Circle: return "Circle";
    case ShapeType::Polygon: return "Polygon";
    case ShapeType::Freehand: return "Freehand";
    }
    return "Unknown";
}

ShapeType Shape::typeFromName(const QString &name)
{
    if (name == "Rectangle") return ShapeType::Rectangle;
    if (name == "Ellipse") return ShapeType::Ellipse;
    if (name == "Circle") return ShapeType::Circle;
    if (name == "Polygon") return ShapeType::Polygon;
    if (name == "Freehand") return ShapeType::Freehand;
    return ShapeType::Line;
}

void Shape::write(QDataStream &out) const
{
    out << static_cast<quint8>(m_type);
    out << m_strokeColor << m_fillColor;
    out << static_cast<qint32>(m_lineWidth);
    out << static_cast<qint32>(m_points.size());
    for (const QPointF &p : m_points)
        out << p;
}

Shape *Shape::read(QDataStream &in)
{
    quint8 rawType = 0;
    in >> rawType;
    if (in.status() != QDataStream::Ok)
        return nullptr;

    Shape *shape = nullptr;
    switch (static_cast<ShapeType>(rawType))
    {
    case ShapeType::Line: shape = new LineShape(); break;
    case ShapeType::Rectangle: shape = new RectangleShape(); break;
    case ShapeType::Ellipse: shape = new EllipseShape(); break;
    case ShapeType::Circle: shape = new CircleShape(); break;
    case ShapeType::Polygon: shape = new PolygonShape(); break;
    case ShapeType::Freehand: shape = new FreehandShape(); break;
    default: return nullptr;
    }

    QColor stroke, fill;
    qint32 width = 0;
    qint32 pointCount = 0;
    in >> stroke >> fill >> width >> pointCount;

    if (in.status() != QDataStream::Ok || pointCount < 0 || pointCount > 100000)
    {
        delete shape;
        return nullptr;
    }

    shape->setStrokeColor(stroke);
    shape->setFillColor(fill);
    shape->setLineWidth(width);

    QVector<QPointF> pts;
    pts.reserve(pointCount);
    for (int i = 0; i < pointCount; ++i)
    {
        QPointF p;
        in >> p;
        if (in.status() != QDataStream::Ok)
        {
            delete shape;
            return nullptr;
        }
        pts.append(p);
    }
    shape->points() = pts;
    return shape;
}
