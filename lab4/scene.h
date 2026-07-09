#ifndef SCENE_H
#define SCENE_H

#include <QVector>
#include <QByteArray>
#include <QString>
#include "shape.h"

class Scene
{
public:
    Scene() = default;
    ~Scene();
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    int count() const { return m_shapes.size(); }
    Shape *at(int index) const;

    void addShape(Shape *shape);
    bool removeAt(int index);
    void clear();

    const QVector<Shape *> &shapes() const { return m_shapes; }

    int hitTestTopmost(const QPointF &pos) const;

    bool bringToFront(int index);
    bool sendToBack(int index);
    bool raise(int index);
    bool lower(int index);

    Scene *cloneAll() const;

    QByteArray toByteArray() const;
    bool fromByteArray(const QByteArray &data, QString *errorMessage = nullptr);

    bool saveToFile(const QString &path, QString *errorMessage = nullptr) const;
    bool loadFromFile(const QString &path, QString *errorMessage = nullptr);

    static constexpr quint32 MAGIC = 0x56474531; // "VGE1"
    static constexpr quint16 VERSION = 1;

private:
    QVector<Shape *> m_shapes;
};

#endif // SCENE_H
