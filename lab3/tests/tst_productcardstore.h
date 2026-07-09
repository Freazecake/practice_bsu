#ifndef TST_PRODUCTCARDSTORE_H
#define TST_PRODUCTCARDSTORE_H

#include <QObject>

class ProductCardStore;

class TestProductCardStore : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testAddRecord();
    void testUpdateRecord();
    void testRemoveRecord();

    void testSaveAndLoadText();
    void testSaveAndLoadBinary();

    void testFindByText();
    void testSortByField();

    void testValidationRejectsInvalidData();
    void testLoadMissingFileFails();
    void testLoadCorruptedTextFails();
    void testLoadCorruptedBinaryFails();

private:
    ProductCardStore *m_store = nullptr;
};

#endif // TST_PRODUCTCARDSTORE_H
