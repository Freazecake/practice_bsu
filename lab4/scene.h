#ifndef SCENE_H
#define SCENE_H

#include <QVector>
#include <QByteArray>
#include <QString>
#include "shape.h"

// Хранит и обслуживает набор графических объектов (владеет указателями).
// Не зависит от GUI — тестируется без QApplication.
class Scene
{
public:
    Scene() = default;
    ~Scene();
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    int count() const { return m_shapes.size(); }
    Shape *at(int index) const;

    void addShape(Shape *shape);          // сцена берёт владение указателем
    bool removeAt(int index);
    void clear();

    // Порядок в векторе = порядок отрисовки (последний элемент — самый верхний слой)
    const QVector<Shape *> &shapes() const { return m_shapes; }

    // Поиск самой верхней фигуры под точкой (для выделения кликом)
    int hitTestTopmost(const QPointF &pos) const;

    // Изменение порядка слоёв
    bool bringToFront(int index);
    bool sendToBack(int index);
    bool raise(int index);   // на один слой выше
    bool lower(int index);   // на один слой ниже

    // Глубокое клонирование всей сцены (используется для undo/redo и copy/paste)
    Scene *cloneAll() const;

    // Сериализация — общий бинарный формат (используется и для файлов, и для undo-снимков)
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
