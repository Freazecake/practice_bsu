#ifndef TST_WAREHOUSEMODEL_H
#define TST_WAREHOUSEMODEL_H

#include <QObject>

class WarehouseModel;

class TestWarehouseModel : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // 1. Добавление новой записи в модель
    void testAddItem();

    // 2. Удаление выбранной записи
    void testRemoveItem();

    // 3. Редактирование данных в ячейке
    void testEditCell();

    // 4. Корректность количества строк и столбцов
    void testRowColumnCount();

    // 5. Сохранение данных в файл (CSV и TXT)
    void testSaveToFileCsv();
    void testSaveToFileTxt();

    // 6. Загрузка данных из файла (CSV и TXT)
    void testLoadFromFileCsv();
    void testLoadFromFileTxt();

    // 7. Обработка некорректных данных
    void testInvalidEditRejected();
    void testInvalidFileContentRejected();
    void testLoadMissingFileFails();

private:
    WarehouseModel *m_model = nullptr;
};

#endif // TST_WAREHOUSEMODEL_H
