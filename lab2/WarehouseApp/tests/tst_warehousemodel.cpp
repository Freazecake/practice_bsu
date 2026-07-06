#include "tst_warehousemodel.h"
#include "../warehousemodel.h"

#include <QtTest>
#include <QTemporaryDir>
#include <QSignalSpy>

void TestWarehouseModel::init()
{
    m_model = new WarehouseModel();
    m_model->addItem({1, "Ноутбук", "Электроника", 15, 45000.00});
    m_model->addItem({2, "Мышь", "Электроника", 120, 500.00});
    m_model->addItem({3, "Стол офисный", "Мебель", 10, 8000.00});
}

void TestWarehouseModel::cleanup()
{
    delete m_model;
    m_model = nullptr;
}

void TestWarehouseModel::testAddItem()
{
    int rowsBefore = m_model->rowCount();

    QSignalSpy insertSpy(m_model, &QAbstractItemModel::rowsInserted);
    m_model->addItem({m_model->nextId(), "Принтер", "Электроника", 8, 12000.00});

    QCOMPARE(m_model->rowCount(), rowsBefore + 1);
    QCOMPARE(insertSpy.count(), 1);

    WarehouseItem added = m_model->itemAt(m_model->rowCount() - 1);
    QCOMPARE(added.name, QString("Принтер"));
    QCOMPARE(added.quantity, 8);
    QCOMPARE(added.price, 12000.00);
}

void TestWarehouseModel::testRemoveItem()
{
    int rowsBefore = m_model->rowCount();
    WarehouseItem toRemove = m_model->itemAt(1);

    QSignalSpy removeSpy(m_model, &QAbstractItemModel::rowsRemoved);
    m_model->removeItem(1);

    QCOMPARE(m_model->rowCount(), rowsBefore - 1);
    QCOMPARE(removeSpy.count(), 1);

    for (int row = 0; row < m_model->rowCount(); ++row){
        QVERIFY(m_model->itemAt(row).id == row + 1);
        QVERIFY(m_model->itemAt(row).name != toRemove.name);
    }
}

void TestWarehouseModel::testEditCell()
{
    QModelIndex nameIndex = m_model->index(0, WarehouseModel::ColName);
    bool ok = m_model->setData(nameIndex, "Ноутбук игровой", Qt::EditRole);

    QVERIFY(ok);
    QCOMPARE(m_model->data(nameIndex, Qt::DisplayRole).toString(), QString("Ноутбук игровой"));

    QModelIndex qtyIndex = m_model->index(0, WarehouseModel::ColQuantity);
    ok = m_model->setData(qtyIndex, 42, Qt::EditRole);
    QVERIFY(ok);
    QCOMPARE(m_model->data(qtyIndex, Qt::EditRole).toInt(), 42);
}

void TestWarehouseModel::testRowColumnCount()
{
    QCOMPARE(m_model->rowCount(), 3);
    QCOMPARE(m_model->columnCount(), static_cast<int>(WarehouseModel::ColumnCount));
    QCOMPARE(m_model->columnCount(), 5);

    m_model->addItem({99, "Тест", "Тест", 1, 1.0});
    QCOMPARE(m_model->rowCount(), 4);
}

void TestWarehouseModel::testSaveToFileCsv()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("test_save.csv");

    QString error;
    bool ok = m_model->saveToCsv(path, &error);

    QVERIFY2(ok, qPrintable(error));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = QString::fromUtf8(file.readAll());
    QVERIFY(content.contains("Ноутбук"));
    QVERIFY(content.contains("ID,Наименование,Категория,Количество,Цена"));
}

void TestWarehouseModel::testSaveToFileTxt()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("test_save.txt");

    QString error;
    bool ok = m_model->saveToTxt(path, &error);

    QVERIFY2(ok, qPrintable(error));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = QString::fromUtf8(file.readAll());
    QVERIFY(content.contains("Мышь"));
    QVERIFY(content.contains("|"));
}

void TestWarehouseModel::testLoadFromFileCsv()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("roundtrip.csv");

    QString error;
    QVERIFY(m_model->saveToCsv(path, &error));

    WarehouseModel loaded;
    bool ok = loaded.loadFromCsv(path, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(loaded.rowCount(), m_model->rowCount());
    QCOMPARE(loaded.itemAt(0).name, m_model->itemAt(0).name);
    QCOMPARE(loaded.itemAt(2).category, QString("Мебель"));
    QCOMPARE(loaded.itemAt(2).quantity, 10);
}

void TestWarehouseModel::testLoadFromFileTxt()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("roundtrip.txt");

    QString error;
    QVERIFY(m_model->saveToTxt(path, &error));

    WarehouseModel loaded;
    bool ok = loaded.loadFromTxt(path, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(loaded.rowCount(), m_model->rowCount());
    QCOMPARE(loaded.itemAt(1).name, QString("Мышь"));
    QCOMPARE(loaded.itemAt(1).price, 500.00);
}

void TestWarehouseModel::testInvalidEditRejected()
{
    QModelIndex nameIndex = m_model->index(0, WarehouseModel::ColName);
    QString originalName = m_model->data(nameIndex).toString();
    bool ok = m_model->setData(nameIndex, "", Qt::EditRole);
    QVERIFY(!ok);
    QCOMPARE(m_model->data(nameIndex).toString(), originalName);

    QModelIndex qtyIndex = m_model->index(0, WarehouseModel::ColQuantity);
    int originalQty = m_model->data(qtyIndex).toInt();
    ok = m_model->setData(qtyIndex, -5, Qt::EditRole);
    QVERIFY(!ok);
    QCOMPARE(m_model->data(qtyIndex).toInt(), originalQty);

    QModelIndex priceIndex = m_model->index(0, WarehouseModel::ColPrice);
    ok = m_model->setData(priceIndex, -100.0, Qt::EditRole);
    QVERIFY(!ok);

    QModelIndex idIndex = m_model->index(0, WarehouseModel::ColId);
    ok = m_model->setData(idIndex, 999, Qt::EditRole);
    QVERIFY(!ok);
    QVERIFY(!(m_model->flags(idIndex) & Qt::ItemIsEditable));
}

void TestWarehouseModel::testInvalidFileContentRejected()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QString badCsvPath = dir.filePath("bad.csv");
    {
        QFile f(badCsvPath);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&f);
        out << "ID,Наименование,Категория,Количество,Цена\n";
        out << "1,Товар,Категория\n";
    }
    QString error;
    bool ok = m_model->loadFromCsv(badCsvPath, &error);
    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());

    QString badTxtPath = dir.filePath("bad.txt");
    {
        QFile f(badTxtPath);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&f);
        out << "1|Товар|Категория|много|1000.00\n";
    }
    ok = m_model->loadFromTxt(badTxtPath, &error);
    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());
}

void TestWarehouseModel::testLoadMissingFileFails()
{
    QString error;
    bool ok = m_model->loadFromCsv("/path/that/does/not/exist_12345.csv", &error);
    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());

    ok = m_model->loadFromTxt("/path/that/does/not/exist_12345.txt", &error);
    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(TestWarehouseModel)
