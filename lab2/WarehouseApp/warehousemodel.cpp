#include "warehousemodel.h"

#include <QFile>
#include <QTextStream>
#include <QBrush>
#include <QColor>
#include <QIODevice>

static void setUtf8(QTextStream &stream)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
}

WarehouseModel::WarehouseModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int WarehouseModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_items.size();
}

int WarehouseModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant WarehouseModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return QVariant();

    const WarehouseItem &item = m_items.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case ColId: return item.id;
        case ColName: return item.name;
        case ColCategory: return item.category;
        case ColQuantity: return item.quantity;
        case ColPrice:
            return role == Qt::EditRole ? QVariant(item.price)
                                         : QVariant(QString::number(item.price, 'f', 2) + " руб.");
        }
    }

    if (role == Qt::BackgroundRole)
    {
        if (item.quantity <= LOW_STOCK_THRESHOLD)
            return QBrush(QColor(105, 105, 5));
    }

    if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == ColQuantity || index.column() == ColPrice || index.column() == ColId)
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
    }

    if (role == Qt::ToolTipRole && item.quantity <= LOW_STOCK_THRESHOLD)
    {
        return QString("Низкий остаток на складе!");
    }

    return QVariant();
}

bool WarehouseModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    WarehouseItem &item = m_items[index.row()];

    switch (index.column())
    {
    case ColId:
        return false;
    case ColName:
    {
        QString name = value.toString().trimmed();
        if (name.isEmpty())
            return false;
        item.name = name;
        break;
    }
    case ColCategory:
        item.category = value.toString().trimmed();
        break;
    case ColQuantity:
    {
        bool ok = false;
        int q = value.toInt(&ok);
        if (!ok || q < 0)
            return false;
        item.quantity = q;
        break;
    }
    case ColPrice:
    {
        bool ok = false;
        double p = value.toDouble(&ok);
        if (!ok || p < 0)
            return false;
        item.price = p;
        break;
    }
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QVariant WarehouseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case ColId: return "ID";
        case ColName: return "Наименование";
        case ColCategory: return "Категория";
        case ColQuantity: return "Кол-во";
        case ColPrice: return "Цена";
        }
    }
    return section + 1;
}

Qt::ItemFlags WarehouseModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() != ColId)
        f |= Qt::ItemIsEditable;
    return f;
}

bool WarehouseModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row > m_items.size())
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_items.insert(row, WarehouseItem{nextId(), "Новый товар", "Без категории", 0, 0.0});
    endInsertRows();
    return true;
}

bool WarehouseModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row + count > m_items.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_items.removeAt(row);
    endRemoveRows();
    updateIds();
    return true;
}

void WarehouseModel::addItem(const WarehouseItem &item)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.append(item);
    endInsertRows();
}

void WarehouseModel::updateItem(int row, const WarehouseItem &item)
{
    if (row < 0 || row >= m_items.size())
        return;
    int id = m_items[row].id;
    m_items[row] = item;
    m_items[row].id = id;
    emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
}

void WarehouseModel::removeItem(int row)
{
    removeRows(row, 1);
}

WarehouseItem WarehouseModel::itemAt(int row) const
{
    if (row < 0 || row >= m_items.size())
        return WarehouseItem();
    return m_items.at(row);
}

int WarehouseModel::totalQuantity() const
{
    int sum = 0;
    for (const auto &it : m_items)
        sum += it.quantity;
    return sum;
}

double WarehouseModel::totalValue() const
{
    double sum = 0.0;
    for (const auto &it : m_items)
        sum += it.total();
    return sum;
}

int WarehouseModel::lowStockCount(int threshold) const
{
    int count = 0;
    for (const auto &it : m_items)
        if (it.quantity <= threshold)
            ++count;
    return count;
}

int WarehouseModel::nextId() const
{
    int maxId = 0;
    for (const auto &it : m_items)
        maxId = qMax(maxId, it.id);
    return maxId + 1;
}

void WarehouseModel::updateIds()
{
    int i = 1;
    for (auto &item : m_items){
        item.id = i;
        ++i;
    }
}

QString WarehouseModel::escapeCsv(const QString &field)
{
    QString f = field;
    if (f.contains(',') || f.contains('"') || f.contains('\n'))
    {
        f.replace("\"", "\"\"");
        f = "\"" + f + "\"";
    }
    return f;
}

QStringList WarehouseModel::splitCsvLine(const QString &line)
{
    QStringList result;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i)
    {
        QChar c = line.at(i);
        if (inQuotes)
        {
            if (c == '"')
            {
                if (i + 1 < line.size() && line.at(i + 1) == '"')
                {
                    field += '"';
                    ++i;
                }
                else
                {
                    inQuotes = false;
                }
            }
            else
            {
                field += c;
            }
        }
        else
        {
            if (c == '"')
                inQuotes = true;
            else if (c == ',')
            {
                result << field;
                field.clear();
            }
            else
            {
                field += c;
            }
        }
    }
    result << field;
    return result;
}

bool WarehouseModel::saveToCsv(const QString &path, QString *errorMessage) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для записи: %1").arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
    setUtf8(out);

    out << "ID,Наименование,Категория,Количество,Цена\n";
    for (const auto &it : m_items)
    {
        out << it.id << ","
            << escapeCsv(it.name) << ","
            << escapeCsv(it.category) << ","
            << it.quantity << ","
            << QString::number(it.price, 'f', 2) << "\n";
    }
    file.close();
    return true;
}

bool WarehouseModel::loadFromCsv(const QString &path, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для чтения: %1").arg(file.errorString());
        return false;
    }

    QTextStream in(&file);
    setUtf8(in);

    QVector<WarehouseItem> newItems;
    bool firstLine = true;
    int lineNo = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        ++lineNo;
        if (line.trimmed().isEmpty())
            continue;
        if (firstLine)
        {
            firstLine = false;
            continue; // пропускаем заголовок
        }

        QStringList fields = splitCsvLine(line);
        if (fields.size() < 5)
        {
            if (errorMessage)
                *errorMessage = QString("Ошибка формата в строке %1 файла CSV").arg(lineNo);
            file.close();
            return false;
        }

        bool okId = false, okQty = false, okPrice = false;
        WarehouseItem item;
        item.id = fields.at(0).toInt(&okId);
        item.name = fields.at(1);
        item.category = fields.at(2);
        item.quantity = fields.at(3).toInt(&okQty);
        QString priceStr = fields.at(4);
        priceStr.replace(',', '.');
        item.price = priceStr.toDouble(&okPrice);

        if (!okId || !okQty || !okPrice || item.name.trimmed().isEmpty())
        {
            if (errorMessage)
                *errorMessage = QString("Некорректные данные в строке %1 файла CSV").arg(lineNo);
            file.close();
            return false;
        }
        newItems.append(item);
    }
    file.close();

    beginResetModel();
    m_items = newItems;
    endResetModel();
    return true;
}

bool WarehouseModel::saveToTxt(const QString &path, QString *errorMessage) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для записи: %1").arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
    setUtf8(out);

    for (const auto &it : m_items)
    {
        out << it.id << "|" << it.name << "|" << it.category << "|"
            << it.quantity << "|" << QString::number(it.price, 'f', 2) << "\n";
    }
    file.close();
    return true;
}

bool WarehouseModel::loadFromTxt(const QString &path, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для чтения: %1").arg(file.errorString());
        return false;
    }

    QTextStream in(&file);
    setUtf8(in);

    QVector<WarehouseItem> newItems;
    int lineNo = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        ++lineNo;
        if (line.trimmed().isEmpty())
            continue;

        QStringList fields = line.split('|');
        if (fields.size() < 5)
        {
            if (errorMessage)
                *errorMessage = QString("Ошибка формата в строке %1 файла TXT").arg(lineNo);
            file.close();
            return false;
        }

        bool okId = false, okQty = false, okPrice = false;
        WarehouseItem item;
        item.id = fields.at(0).toInt(&okId);
        item.name = fields.at(1);
        item.category = fields.at(2);
        item.quantity = fields.at(3).toInt(&okQty);
        QString priceStr = fields.at(4);
        priceStr.replace(',', '.');
        item.price = priceStr.toDouble(&okPrice);

        if (!okId || !okQty || !okPrice || item.name.trimmed().isEmpty())
        {
            if (errorMessage)
                *errorMessage = QString("Некорректные данные в строке %1 файла TXT").arg(lineNo);
            file.close();
            return false;
        }
        newItems.append(item);
    }
    file.close();

    beginResetModel();
    m_items = newItems;
    endResetModel();
    return true;
}
