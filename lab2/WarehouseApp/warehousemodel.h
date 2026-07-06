#ifndef WAREHOUSEMODEL_H
#define WAREHOUSEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

struct WarehouseItem
{
    int id = 0;
    QString name;
    QString category;
    int quantity = 0;
    double price = 0.0;

    double total() const { return quantity * price; }
};

class WarehouseModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        ColId = 0,
        ColName,
        ColCategory,
        ColQuantity,
        ColPrice,
        ColumnCount
    };

    explicit WarehouseModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void addItem(const WarehouseItem &item);
    void updateItem(int row, const WarehouseItem &item);
    void removeItem(int row);
    WarehouseItem itemAt(int row) const;

    int totalQuantity() const;
    double totalValue() const;
    int lowStockCount(int threshold) const;

    bool saveToCsv(const QString &path, QString *errorMessage = nullptr) const;
    bool loadFromCsv(const QString &path, QString *errorMessage = nullptr);
    bool saveToTxt(const QString &path, QString *errorMessage = nullptr) const;
    bool loadFromTxt(const QString &path, QString *errorMessage = nullptr);

    int nextId() const;


    static const int LOW_STOCK_THRESHOLD = 10;

private:
    QVector<WarehouseItem> m_items;

    static QString escapeCsv(const QString &field);
    static QStringList splitCsvLine(const QString &line);
    void updateIds();
};

#endif // WAREHOUSEMODEL_H
