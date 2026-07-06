#ifndef TST_PRODUCTCARDSTORE_H
#define TST_PRODUCTCARDSTORE_H

#include <QObject>

class ProductCardStore;

// Отдельный тестовый класс (QTest) — тестирует бизнес-логику без GUI
class TestProductCardStore : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // 1. Создание и изменение данных
    void testAddRecord();
    void testUpdateRecord();
    void testRemoveRecord();

    // 2. Сохранение и загрузка файлов (текстовый и бинарный форматы)
    void testSaveAndLoadText();
    void testSaveAndLoadBinary();

    // Поиск и сортировка (доп. функциональность)
    void testFindByText();
    void testSortByField();

    // 3. Обработка ошибок
    void testValidationRejectsInvalidData();
    void testLoadMissingFileFails();
    void testLoadCorruptedTextFails();
    void testLoadCorruptedBinaryFails();

private:
    ProductCardStore *m_store = nullptr;
};

#endif // TST_PRODUCTCARDSTORE_H
