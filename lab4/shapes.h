#ifndef SHAPES_H
#define SHAPES_H

#include "shape.h"

// Линия: points = [начало, конец]
class LineShape : public Shape
{
public:
    LineShape() : Shape(ShapeType::Line) {}
    void draw(QPainter &painter) const override;
    bool hitTest(const QPointF &pos) const override;
    QRectF boundingRect() const override;
    Shape *clone() const override;
    QVector<QPointF> resizeHandles() const override;
};

// Прямоугольник: points = [левый верхний угол, правый нижний угол]
class RectangleShape : public Shape
{
public:
    RectangleShape() : Shape(ShapeType::Rectangle) {}
    void draw(QPainter &painter) const override;
    bool hitTest(const QPointF &pos) const override;
    QRectF boundingRect() const override;
    Shape *clone() const override;
    QVector<QPointF> resizeHandles() const override;
    int pointIndexForHandle(int handleIndex) const override { Q_UNUSED(handleIndex); return 1; }
};

// Эллипс: points = [левый верхний угол ограничивающего прямоугольника, правый нижний]
class EllipseShape : public Shape
{
public:
    EllipseShape() : Shape(ShapeType::Ellipse) {}
    void draw(QPainter &painter) const override;
    bool hitTest(const QPointF &pos) const override;
    QRectF boundingRect() const override;
    Shape *clone() const override;
    QVector<QPointF> resizeHandles() const override;
    int pointIndexForHandle(int handleIndex) const override { Q_UNUSED(handleIndex); return 1; }
};

// Окружность: points = [центр, точка на окружности (задаёт радиус)]
class CircleShape : public Shape
{
public:
    CircleShape() : Shape(ShapeType::Circle) {}
    void draw(QPainter &painter) const override;
    bool hitTest(const QPointF &pos) const override;
    QRectF boundingRect() const override;
    Shape *clone() const override;
    QVector<QPointF> resizeHandles() const override;
    int pointIndexForHandle(int handleIndex) const override { Q_UNUSED(handleIndex); return 1; }

    qreal radius() const;
};

// Многоугольник: points = вершины (строится последовательными кликами)
class PolygonShape : public Shape
{
public:
    PolygonShape() : Shape(ShapeType::Polygon) {}
    void draw(QPainter &painter) const override;
    bool hitTest(const QPointF &pos) const override;
    QRectF boundingRect() const override;
    Shape *clone() const override;
};

// Свободная кривая: points = последовательность точек, собранных при движении мыши
class FreehandShape : public Shape
{
public:
    FreehandShape() : Shape(ShapeType::Freehand) {}
    void draw(QPainter &painter) const override;
    bool hitTest(const QPointF &pos) const override;
    QRectF boundingRect() const override;
    Shape *clone() const override;
};

#endif // SHAPES_H
