#ifndef PRODUCTCARDSTORE_H
#define PRODUCTCARDSTORE_H

#include <QObject>
#include <QVector>
#include "productcard.h"

class ProductCardStore : public QObject
{
    Q_OBJECT

public:
    enum class SortField
    {
        Id,
        Name,
        Category,
        Quantity,
        Price,
        ReceivedDate
    };

    explicit ProductCardStore(QObject *parent = nullptr);

    int count() const;
    ProductCard at(int index) const;
    int nextId() const;

    int addRecord(const ProductCard &card);
    bool updateRecord(int index, const ProductCard &card);
    bool removeRecord(int index);
    void clear();

    QVector<int> findByText(const QString &query) const;

    void sortBy(SortField field, Qt::SortOrder order = Qt::AscendingOrder);

    static bool validate(const ProductCard &card, QString *errorMessage = nullptr);

    bool saveText(const QString &path, QString *errorMessage = nullptr) const;
    bool loadText(const QString &path, QString *errorMessage = nullptr);

    bool saveBinary(const QString &path, QString *errorMessage = nullptr) const;
    bool loadBinary(const QString &path, QString *errorMessage = nullptr);

signals:
    void recordsChanged();

private:
    QVector<ProductCard> m_cards;
};

#endif // PRODUCTCARDSTORE_H
