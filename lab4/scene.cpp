#include "scene.h"

#include <QFile>
#include <QIODevice>
#include <QBuffer>

Scene::~Scene()
{
    clear();
}

Shape *Scene::at(int index) const
{
    if (index < 0 || index >= m_shapes.size())
        return nullptr;
    return m_shapes.at(index);
}

void Scene::addShape(Shape *shape)
{
    m_shapes.append(shape);
}

bool Scene::removeAt(int index)
{
    if (index < 0 || index >= m_shapes.size())
        return false;
    delete m_shapes.at(index);
    m_shapes.remove(index);
    return true;
}

void Scene::clear()
{
    qDeleteAll(m_shapes);
    m_shapes.clear();
}

int Scene::hitTestTopmost(const QPointF &pos) const
{
    for (int i = m_shapes.size() - 1; i >= 0; --i)
        if (m_shapes.at(i)->hitTest(pos))
            return i;
    return -1;
}

bool Scene::bringToFront(int index)
{
    if (index < 0 || index >= m_shapes.size() || index == m_shapes.size() - 1)
        return false;
    Shape *s = m_shapes.takeAt(index);
    m_shapes.append(s);
    return true;
}

bool Scene::sendToBack(int index)
{
    if (index <= 0 || index >= m_shapes.size())
        return false;
    Shape *s = m_shapes.takeAt(index);
    m_shapes.prepend(s);
    return true;
}

bool Scene::raise(int index)
{
    if (index < 0 || index >= m_shapes.size() - 1)
        return false;
    m_shapes.swapItemsAt(index, index + 1);
    return true;
}

bool Scene::lower(int index)
{
    if (index <= 0 || index >= m_shapes.size())
        return false;
    m_shapes.swapItemsAt(index, index - 1);
    return true;
}

Scene *Scene::cloneAll() const
{
    auto *copy = new Scene();
    for (Shape *s : m_shapes)
        copy->addShape(s->clone());
    return copy;
}

QByteArray Scene::toByteArray() const
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << MAGIC << VERSION;
    out << static_cast<quint32>(m_shapes.size());
    for (Shape *s : m_shapes)
        s->write(out);

    return data;
}

bool Scene::fromByteArray(const QByteArray &data, QString *errorMessage)
{
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_5_15);

    quint32 magic = 0;
    quint16 version = 0;
    in >> magic >> version;

    if (in.status() != QDataStream::Ok || magic != MAGIC)
    {
        if (errorMessage)
            *errorMessage = "Файл повреждён или не является файлом векторного редактора (VGE1).";
        return false;
    }
    if (version != VERSION)
    {
        if (errorMessage)
            *errorMessage = QString("Неподдерживаемая версия файла: %1.").arg(version);
        return false;
    }

    quint32 shapeCount = 0;
    in >> shapeCount;
    if (in.status() != QDataStream::Ok || shapeCount > 1000000)
    {
        if (errorMessage)
            *errorMessage = "Файл повреждён: некорректное количество объектов.";
        return false;
    }

    QVector<Shape *> newShapes;
    newShapes.reserve(shapeCount);
    for (quint32 i = 0; i < shapeCount; ++i)
    {
        Shape *s = Shape::read(in);
        if (!s || in.status() != QDataStream::Ok)
        {
            if (errorMessage)
                *errorMessage = QString("Ошибка чтения объекта №%1 — файл повреждён.").arg(i + 1);
            qDeleteAll(newShapes);
            delete s;
            return false;
        }
        newShapes.append(s);
    }

    clear();
    m_shapes = newShapes;
    return true;
}

bool Scene::saveToFile(const QString &path, QString *errorMessage) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для записи: %1").arg(file.errorString());
        return false;
    }
    file.write(toByteArray());
    file.close();
    return true;
}

bool Scene::loadFromFile(const QString &path, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для чтения: %1").arg(file.errorString());
        return false;
    }
    QByteArray data = file.readAll();
    file.close();
    return fromByteArray(data, errorMessage);
}
