#include "productcard.h"

QDataStream &operator<<(QDataStream &out, const ProductCard &card)
{
    out << card.id << card.name << card.category << card.quantity << card.price << card.receivedDate;
    return out;
}

QDataStream &operator>>(QDataStream &in, ProductCard &card)
{
    in >> card.id >> card.name >> card.category >> card.quantity >> card.price >> card.receivedDate;
    return in;
}
