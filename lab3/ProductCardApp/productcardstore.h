#ifndef PRODUCTCARDSTORE_H
#define PRODUCTCARDSTORE_H

#include <QObject>
#include <QVector>
#include "productcard.h"

// Бизнес-логика хранилища карточек товара, полностью отделена от GUI —
// это позволяет модульно тестировать её без создания окон (QApplication).
class ProductCardStore : public QObject
{
    Q_OBJECT

public:
    enum class SortField { Id, Name, Category, Quantity, Price, ReceivedDate };

    explicit ProductCardStore(QObject *parent = nullptr);

    int count() const;
    ProductCard at(int index) const;
    int nextId() const;

    int addRecord(const ProductCard &card);          // возвращает индекс новой записи
    bool updateRecord(int index, const ProductCard &card);
    bool removeRecord(int index);
    void clear();

    // Поиск по нескольким полям (наименование, категория, id) — регистронезависимый
    QVector<int> findByText(const QString &query) const;

    // Сортировка записей по выбранному полю
    void sortBy(SortField field, Qt::SortOrder order = Qt::AscendingOrder);

    // Проверка корректности данных записи (пустые поля, диапазоны значений)
    static bool validate(const ProductCard &card, QString *errorMessage = nullptr);

    // Сохранение / загрузка — текстовый формат (QTextStream, разделитель '|')
    bool saveText(const QString &path, QString *errorMessage = nullptr) const;
    bool loadText(const QString &path, QString *errorMessage = nullptr);

    // Сохранение / загрузка — бинарный формат (QDataStream)
    bool saveBinary(const QString &path, QString *errorMessage = nullptr) const;
    bool loadBinary(const QString &path, QString *errorMessage = nullptr);

signals:
    void recordsChanged();

private:
    QVector<ProductCard> m_cards;
};

#endif // PRODUCTCARDSTORE_H
