#include "tst_productcardstore.h"
#include "../productcardstore.h"

#include <QtTest>
#include <QTemporaryDir>

void TestProductCardStore::init()
{
    m_store = new ProductCardStore();
    m_store->addRecord({1, "Ноутбук", "Электроника", 15, 45000.00, QDate(2026, 3, 10)});
    m_store->addRecord({2, "Стол офисный", "Мебель", 10, 8000.00, QDate(2026, 4, 2)});
    m_store->addRecord({3, "Принтер", "Электроника", 8, 12000.00, QDate(2026, 5, 20)});
}

void TestProductCardStore::cleanup()
{
    delete m_store;
    m_store = nullptr;
}

void TestProductCardStore::testAddRecord()
{
    int countBefore = m_store->count();
    QSignalSpy spy(m_store, &ProductCardStore::recordsChanged);

    int idx = m_store->addRecord({m_store->nextId(), "Мышь", "Электроника", 100, 500.0, QDate::currentDate()});

    QCOMPARE(m_store->count(), countBefore + 1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_store->at(idx).name, QString("Мышь"));
}

void TestProductCardStore::testUpdateRecord()
{
    ProductCard updated = m_store->at(0);
    updated.name = "Ноутбук игровой";
    updated.quantity = 20;

    bool ok = m_store->updateRecord(0, updated);

    QVERIFY(ok);
    QCOMPARE(m_store->at(0).name, QString("Ноутбук игровой"));
    QCOMPARE(m_store->at(0).quantity, 20);
    QCOMPARE(m_store->at(0).id, 1); // id не должен меняться при редактировании
}

void TestProductCardStore::testRemoveRecord()
{
    int countBefore = m_store->count();
    bool ok = m_store->removeRecord(1); // "Стол офисный"

    QVERIFY(ok);
    QCOMPARE(m_store->count(), countBefore - 1);
    for (int i = 0; i < m_store->count(); ++i)
        QVERIFY(m_store->at(i).name != QString("Стол офисный"));
}

void TestProductCardStore::testSaveAndLoadText()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("products.txt");

    QString error;
    QVERIFY2(m_store->saveText(path, &error), qPrintable(error));
    QVERIFY(QFile::exists(path));

    ProductCardStore loaded;
    QVERIFY2(loaded.loadText(path, &error), qPrintable(error));

    QCOMPARE(loaded.count(), m_store->count());
    QCOMPARE(loaded.at(0).name, m_store->at(0).name);
    QCOMPARE(loaded.at(1).quantity, m_store->at(1).quantity);
    QCOMPARE(loaded.at(2).receivedDate, m_store->at(2).receivedDate);
}

void TestProductCardStore::testSaveAndLoadBinary()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("products.dat");

    QString error;
    QVERIFY2(m_store->saveBinary(path, &error), qPrintable(error));
    QVERIFY(QFile::exists(path));

    ProductCardStore loaded;
    QVERIFY2(loaded.loadBinary(path, &error), qPrintable(error));

    QCOMPARE(loaded.count(), m_store->count());
    QCOMPARE(loaded.at(0).name, m_store->at(0).name);
    QCOMPARE(loaded.at(2).price, m_store->at(2).price);
    QCOMPARE(loaded.at(1).receivedDate, m_store->at(1).receivedDate);
}

void TestProductCardStore::testFindByText()
{
    QVector<int> byName = m_store->findByText("ноутбук");
    QCOMPARE(byName.size(), 1);
    QCOMPARE(m_store->at(byName.first()).id, 1);

    QVector<int> byCategory = m_store->findByText("Электроника");
    QCOMPARE(byCategory.size(), 2); // Ноутбук + Принтер

    QVector<int> byId = m_store->findByText("2");
    QCOMPARE(byId.size(), 1);
    QCOMPARE(m_store->at(byId.first()).name, QString("Стол офисный"));
}

void TestProductCardStore::testSortByField()
{
    m_store->sortBy(ProductCardStore::SortField::Quantity, Qt::AscendingOrder);
    QVERIFY(m_store->at(0).quantity <= m_store->at(1).quantity);
    QVERIFY(m_store->at(1).quantity <= m_store->at(2).quantity);

    m_store->sortBy(ProductCardStore::SortField::Price, Qt::DescendingOrder);
    QVERIFY(m_store->at(0).price >= m_store->at(1).price);
    QVERIFY(m_store->at(1).price >= m_store->at(2).price);
}

void TestProductCardStore::testValidationRejectsInvalidData()
{
    QString error;

    ProductCard emptyName;
    emptyName.name = "";
    emptyName.category = "Категория";
    QVERIFY(!ProductCardStore::validate(emptyName, &error));
    QVERIFY(!error.isEmpty());

    ProductCard negativeQty;
    negativeQty.name = "Товар";
    negativeQty.category = "Категория";
    negativeQty.quantity = -5;
    QVERIFY(!ProductCardStore::validate(negativeQty, &error));

    ProductCard negativePrice;
    negativePrice.name = "Товар";
    negativePrice.category = "Категория";
    negativePrice.price = -100.0;
    QVERIFY(!ProductCardStore::validate(negativePrice, &error));

    ProductCard validCard;
    validCard.name = "Товар";
    validCard.category = "Категория";
    validCard.quantity = 5;
    validCard.price = 100.0;
    QVERIFY(ProductCardStore::validate(validCard, &error));
}

void TestProductCardStore::testLoadMissingFileFails()
{
    QString error;
    QVERIFY(!m_store->loadText("/no/such/path/missing_12345.txt", &error));
    QVERIFY(!error.isEmpty());

    QVERIFY(!m_store->loadBinary("/no/such/path/missing_12345.dat", &error));
    QVERIFY(!error.isEmpty());
}

void TestProductCardStore::testLoadCorruptedTextFails()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("bad.txt");

    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&f);
        out << "1|Товар|Категория\n"; // не хватает полей (количество/цена/дата)
    }

    QString error;
    QVERIFY(!m_store->loadText(path, &error));
    QVERIFY(!error.isEmpty());
}

void TestProductCardStore::testLoadCorruptedBinaryFails()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("bad.dat");

    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        QDataStream out(&f);
        out << QString("это не карточка товара, а случайные данные");
    }

    QString error;
    QVERIFY(!m_store->loadBinary(path, &error));
    QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(TestProductCardStore)
