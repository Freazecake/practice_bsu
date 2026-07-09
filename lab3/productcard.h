#ifndef PRODUCTCARD_H
#define PRODUCTCARD_H

#include <QString>
#include <QDate>
#include <QDataStream>

struct ProductCard
{
    int id = 0;
    QString name;
    QString category;
    int quantity = 0;
    double price = 0.0;
    QDate receivedDate = QDate::currentDate();

    double total() const { return quantity * price; }
};

constexpr quint32 PRODUCT_CARD_MAGIC = 0x50435330;
constexpr quint16 PRODUCT_CARD_VERSION = 1;

QDataStream &operator<<(QDataStream &out, const ProductCard &card);
QDataStream &operator>>(QDataStream &in, ProductCard &card);

#endif // PRODUCTCARD_H
