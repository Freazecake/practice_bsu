#include "productcardstore.h"

#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QIODevice>
#include <algorithm>

ProductCardStore::ProductCardStore(QObject *parent)
    : QObject(parent)
{
}

int ProductCardStore::count() const
{
    return m_cards.size();
}

ProductCard ProductCardStore::at(int index) const
{
    if (index < 0 || index >= m_cards.size())
        return ProductCard();
    return m_cards.at(index);
}

int ProductCardStore::nextId() const
{
    int maxId = 0;
    for (const auto &c : m_cards)
        maxId = std::max(maxId, c.id);
    return maxId + 1;
}

int ProductCardStore::addRecord(const ProductCard &card)
{
    m_cards.append(card);
    emit recordsChanged();
    return m_cards.size() - 1;
}

bool ProductCardStore::updateRecord(int index, const ProductCard &card)
{
    if (index < 0 || index >= m_cards.size())
        return false;
    int id = m_cards[index].id;
    m_cards[index] = card;
    m_cards[index].id = id;
    emit recordsChanged();
    return true;
}

bool ProductCardStore::removeRecord(int index)
{
    if (index < 0 || index >= m_cards.size())
        return false;
    m_cards.removeAt(index);
    emit recordsChanged();
    return true;
}

void ProductCardStore::clear()
{
    m_cards.clear();
    emit recordsChanged();
}

QVector<int> ProductCardStore::findByText(const QString &query) const
{
    QVector<int> result;
    if (query.trimmed().isEmpty())
    {
        for (int i = 0; i < m_cards.size(); ++i)
            result << i;
        return result;
    }

    const QString needle = query.trimmed();
    for (int i = 0; i < m_cards.size(); ++i)
    {
        const ProductCard &c = m_cards.at(i);
        if (c.name.contains(needle, Qt::CaseInsensitive) ||
            c.category.contains(needle, Qt::CaseInsensitive) ||
            QString::number(c.id) == needle)
        {
            result << i;
        }
    }
    return result;
}

static bool isLess(const ProductCard &a, const ProductCard &b, ProductCardStore::SortField field)
{
    switch (field)
    {
    case ProductCardStore::SortField::Id:
        return a.id < b.id;
    case ProductCardStore::SortField::Name:
        return a.name.localeAwareCompare(b.name) < 0;
    case ProductCardStore::SortField::Category:
        return a.category.localeAwareCompare(b.category) < 0;
    case ProductCardStore::SortField::Quantity:
        return a.quantity < b.quantity;
    case ProductCardStore::SortField::Price:
        return a.price < b.price;
    case ProductCardStore::SortField::ReceivedDate:
        return a.receivedDate < b.receivedDate;
    }
    return false;
}

void ProductCardStore::sortBy(SortField field, Qt::SortOrder order)
{
    std::sort(m_cards.begin(), m_cards.end(), [field, order](const ProductCard &a, const ProductCard &b)
              { return order == Qt::AscendingOrder ? isLess(a, b, field) : isLess(b, a, field); });
    emit recordsChanged();
}

bool ProductCardStore::validate(const ProductCard &card, QString *errorMessage)
{
    if (card.name.trimmed().isEmpty())
    {
        if (errorMessage)
            *errorMessage = "Наименование товара не может быть пустым.";
        return false;
    }
    if (card.category.trimmed().isEmpty())
    {
        if (errorMessage)
            *errorMessage = "Категория не может быть пустой.";
        return false;
    }
    if (card.quantity < 0 || card.quantity > 1000000)
    {
        if (errorMessage)
            *errorMessage = "Количество должно быть в диапазоне от 0 до 1 000 000.";
        return false;
    }
    if (card.price < 0.0 || card.price > 100000000.0)
    {
        if (errorMessage)
            *errorMessage = "Цена должна быть в диапазоне от 0 до 100 000 000.";
        return false;
    }
    if (!card.receivedDate.isValid())
    {
        if (errorMessage)
            *errorMessage = "Некорректная дата поступления.";
        return false;
    }
    return true;
}

static QString escapeField(const QString &field)
{
    QString f = field;
    f.replace('|', "\\|");
    f.replace('\n', ' ');
    return f;
}

static QString unescapeField(const QString &field)
{
    QString f = field;
    f.replace("\\|", "|");
    return f;
}

bool ProductCardStore::saveText(const QString &path, QString *errorMessage) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для записи: %1").arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    for (const auto &c : m_cards)
    {
        out << c.id << '|'
            << escapeField(c.name) << '|'
            << escapeField(c.category) << '|'
            << c.quantity << '|'
            << QString::number(c.price, 'f', 2) << '|'
            << c.receivedDate.toString(Qt::ISODate) << '\n';
    }
    file.close();
    return true;
}

bool ProductCardStore::loadText(const QString &path, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для чтения: %1").arg(file.errorString());
        return false;
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    QVector<ProductCard> newCards;
    int lineNo = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        ++lineNo;
        if (line.trimmed().isEmpty())
            continue;

        QStringList fields = line.split('|');
        if (fields.size() < 6)
        {
            if (errorMessage)
                *errorMessage = QString("Ошибка формата в строке %1 текстового файла.").arg(lineNo);
            file.close();
            return false;
        }

        bool okId = false, okQty = false, okPrice = false;
        ProductCard card;
        card.id = fields.at(0).toInt(&okId);
        card.name = unescapeField(fields.at(1));
        card.category = unescapeField(fields.at(2));
        card.quantity = fields.at(3).toInt(&okQty);
        card.price = fields.at(4).toDouble(&okPrice);
        card.receivedDate = QDate::fromString(fields.at(5), Qt::ISODate);

        if (!okId || !okQty || !okPrice || !card.receivedDate.isValid())
        {
            if (errorMessage)
                *errorMessage = QString("Некорректные данные в строке %1 текстового файла.").arg(lineNo);
            file.close();
            return false;
        }
        newCards.append(card);
    }
    file.close();

    m_cards = newCards;
    emit recordsChanged();
    return true;
}

bool ProductCardStore::saveBinary(const QString &path, QString *errorMessage) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для записи: %1").arg(file.errorString());
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    out << PRODUCT_CARD_MAGIC << PRODUCT_CARD_VERSION;
    out << static_cast<quint32>(m_cards.size());
    for (const auto &c : m_cards)
        out << c;

    file.close();
    return true;
}

bool ProductCardStore::loadBinary(const QString &path, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для чтения: %1").arg(file.errorString());
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    quint32 magic = 0;
    quint16 version = 0;
    in >> magic >> version;

    if (in.status() != QDataStream::Ok || magic != PRODUCT_CARD_MAGIC)
    {
        if (errorMessage)
            *errorMessage = "Файл повреждён или имеет неверный формат (не бинарный файл карточек товара).";
        file.close();
        return false;
    }
    if (version != PRODUCT_CARD_VERSION)
    {
        if (errorMessage)
            *errorMessage = QString("Неподдерживаемая версия бинарного файла: %1.").arg(version);
        file.close();
        return false;
    }

    quint32 recordCount = 0;
    in >> recordCount;

    QVector<ProductCard> newCards;
    newCards.reserve(recordCount);
    for (quint32 i = 0; i < recordCount; ++i)
    {
        ProductCard card;
        in >> card;
        if (in.status() != QDataStream::Ok)
        {
            if (errorMessage)
                *errorMessage = QString("Ошибка чтения записи №%1 из бинарного файла — файл повреждён.").arg(i + 1);
            file.close();
            return false;
        }
        newCards.append(card);
    }
    file.close();

    m_cards = newCards;
    emit recordsChanged();
    return true;
}
