#include "mainwindow.h"
#include "warehousemodel.h"
#include "itemdialog.h"

#include <QTableView>
#include <QSortFilterProxyModel>
#include <QToolBar>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QStatusBar>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QAction>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_model = new WarehouseModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterKeyColumn(WarehouseModel::ColName);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    setupUi();
    setupSampleData();
    refreshCategoryFilter();
    updateStatusBar();

    setWindowTitle("Учёт товаров на складе");
    resize(900, 560);
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // --- Панель поиска и фильтрации (дополнительная возможность) ---
    auto *filterBox = new QGroupBox("Поиск и фильтр", central);
    auto *filterLayout = new QHBoxLayout(filterBox);

    m_searchEdit = new QLineEdit(filterBox);
    m_searchEdit->setPlaceholderText("Поиск по наименованию...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    m_categoryFilter = new QComboBox(filterBox);
    connect(m_categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCategoryFilterChanged);

    filterLayout->addWidget(new QLabel("Наименование:"));
    filterLayout->addWidget(m_searchEdit, 1);
    filterLayout->addWidget(new QLabel("Категория:"));
    filterLayout->addWidget(m_categoryFilter);

    // --- Таблица ---
    m_tableView = new QTableView(central);
    m_tableView->setModel(m_proxyModel);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);      // сортировка по клику на заголовок
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(WarehouseModel::ColName, QHeaderView::Stretch);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setAlternatingRowColors(true);
    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onEditItem);

    // --- Кнопки управления записями ---
    auto *recordBox = new QGroupBox("Записи", central);
    auto *recordLayout = new QHBoxLayout(recordBox);
    auto *addBtn = new QPushButton("Добавить", recordBox);
    auto *editBtn = new QPushButton("Редактировать", recordBox);
    auto *delBtn = new QPushButton("Удалить", recordBox);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddItem);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditItem);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteItem);
    recordLayout->addWidget(addBtn);
    recordLayout->addWidget(editBtn);
    recordLayout->addWidget(delBtn);
    recordLayout->addStretch();

    // --- Кнопки сохранения / загрузки ---
    auto *fileBox = new QGroupBox("Файл (2 формата: TXT и CSV)", central);
    auto *fileLayout = new QHBoxLayout(fileBox);
    auto *saveTxtBtn = new QPushButton("Сохранить TXT", fileBox);
    auto *loadTxtBtn = new QPushButton("Загрузить TXT", fileBox);
    auto *saveCsvBtn = new QPushButton("Сохранить CSV", fileBox);
    auto *loadCsvBtn = new QPushButton("Загрузить CSV", fileBox);
    connect(saveTxtBtn, &QPushButton::clicked, this, &MainWindow::onSaveTxt);
    connect(loadTxtBtn, &QPushButton::clicked, this, &MainWindow::onLoadTxt);
    connect(saveCsvBtn, &QPushButton::clicked, this, &MainWindow::onSaveCsv);
    connect(loadCsvBtn, &QPushButton::clicked, this, &MainWindow::onLoadCsv);
    fileLayout->addWidget(saveTxtBtn);
    fileLayout->addWidget(loadTxtBtn);
    fileLayout->addWidget(saveCsvBtn);
    fileLayout->addWidget(loadCsvBtn);
    fileLayout->addStretch();

    mainLayout->addWidget(filterBox);
    mainLayout->addWidget(m_tableView, 1);
    mainLayout->addWidget(recordBox);
    mainLayout->addWidget(fileBox);

    setCentralWidget(central);

    // --- Статистика в строке состояния (дополнительная возможность) ---
    m_statsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_statsLabel);

    connect(m_model, &WarehouseModel::dataChanged, this, &MainWindow::updateStatusBar);
    connect(m_model, &WarehouseModel::rowsInserted, this, &MainWindow::updateStatusBar);
    connect(m_model, &WarehouseModel::rowsRemoved, this, &MainWindow::updateStatusBar);
    connect(m_model, &WarehouseModel::modelReset, this, [this]() {
        refreshCategoryFilter();
        updateStatusBar();
    });
    connect(m_model, &WarehouseModel::rowsInserted, this, &MainWindow::refreshCategoryFilter);
    connect(m_model, &WarehouseModel::dataChanged, this, &MainWindow::refreshCategoryFilter);
}

void MainWindow::setupSampleData()
{
    // Не менее 10 записей, не менее 5 столбцов (ID, Наименование, Категория, Кол-во, Цена)
    m_model->addItem({1, "Ноутбук", "Электроника", 15, 45000.00});
    m_model->addItem({2, "Мышь компьютерная", "Электроника", 120, 500.00});
    m_model->addItem({3, "Клавиатура", "Электроника", 80, 1200.00});
    m_model->addItem({4, "Монитор 24\"", "Электроника", 25, 15000.00});
    m_model->addItem({5, "Стол офисный", "Мебель", 10, 8000.00});
    m_model->addItem({6, "Стул офисный", "Мебель", 30, 3500.00});
    m_model->addItem({7, "Бумага А4 (пачка)", "Канцелярия", 200, 250.00});
    m_model->addItem({8, "Ручка шариковая", "Канцелярия", 500, 20.00});
    m_model->addItem({9, "Принтер лазерный", "Электроника", 8, 12000.00});
    m_model->addItem({10, "Шкаф архивный", "Мебель", 5, 9500.00});
}

void MainWindow::refreshCategoryFilter()
{
    QString current = m_categoryFilter->currentText();

    m_categoryFilter->blockSignals(true);
    m_categoryFilter->clear();
    m_categoryFilter->addItem("Все категории");

    QStringList categories;
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        QString cat = m_model->itemAt(row).category;
        if (!cat.isEmpty() && !categories.contains(cat))
            categories << cat;
    }
    categories.sort();
    m_categoryFilter->addItems(categories);

    int idx = m_categoryFilter->findText(current);
    m_categoryFilter->setCurrentIndex(idx >= 0 ? idx : 0);
    m_categoryFilter->blockSignals(false);
}

int MainWindow::currentSourceRow() const
{
    QModelIndex proxyIndex = m_tableView->currentIndex();
    if (!proxyIndex.isValid())
        return -1;
    return m_proxyModel->mapToSource(proxyIndex).row();
}

void MainWindow::onAddItem()
{
    ItemDialog dlg(this);
    WarehouseItem blank;
    blank.category = m_categoryFilter->currentText() == "Все категории" ? "" : m_categoryFilter->currentText();
    dlg.setItem(blank);

    if (dlg.exec() == QDialog::Accepted)
    {
        WarehouseItem item = dlg.item();
        item.id = m_model->nextId();
        m_model->addItem(item);
    }
}

void MainWindow::onEditItem()
{
    int row = currentSourceRow();
    if (row < 0)
    {
        QMessageBox::information(this, "Редактирование", "Выберите запись в таблице для редактирования.");
        return;
    }

    ItemDialog dlg(this);
    dlg.setItem(m_model->itemAt(row));
    if (dlg.exec() == QDialog::Accepted)
    {
        m_model->updateItem(row, dlg.item());
    }
}

void MainWindow::onDeleteItem()
{
    int row = currentSourceRow();
    if (row < 0)
    {
        QMessageBox::information(this, "Удаление", "Выберите запись в таблице для удаления.");
        return;
    }

    WarehouseItem item = m_model->itemAt(row);
    auto reply = QMessageBox::question(this, "Подтверждение удаления",
                                        QString("Удалить запись \"%1\"?").arg(item.name),
                                        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        m_model->removeItem(row);
}

void MainWindow::onSaveTxt()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить в TXT", "warehouse.txt", "Текстовые файлы (*.txt)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_model->saveToTxt(path, &error))
        QMessageBox::critical(this, "Ошибка сохранения", error);
    else
        statusBar()->showMessage("Данные сохранены в " + path, 4000);
}

void MainWindow::onLoadTxt()
{
    QString path = QFileDialog::getOpenFileName(this, "Загрузить из TXT", QString(), "Текстовые файлы (*.txt)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_model->loadFromTxt(path, &error))
        QMessageBox::critical(this, "Ошибка загрузки", error);
    else
        statusBar()->showMessage("Данные загружены из " + path, 4000);
}

void MainWindow::onSaveCsv()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить в CSV", "warehouse.csv", "CSV файлы (*.csv)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_model->saveToCsv(path, &error))
        QMessageBox::critical(this, "Ошибка сохранения", error);
    else
        statusBar()->showMessage("Данные сохранены в " + path, 4000);
}

void MainWindow::onLoadCsv()
{
    QString path = QFileDialog::getOpenFileName(this, "Загрузить из CSV", QString(), "CSV файлы (*.csv)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_model->loadFromCsv(path, &error))
        QMessageBox::critical(this, "Ошибка загрузки", error);
    else
        statusBar()->showMessage("Данные загружены из " + path, 4000);
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}

void MainWindow::onCategoryFilterChanged(int index)
{
    Q_UNUSED(index);
    QString category = m_categoryFilter->currentText();

    if (category == "Все категории" || category.isEmpty())
    {
        m_proxyModel->setFilterKeyColumn(WarehouseModel::ColName);
        m_proxyModel->setFilterFixedString(m_searchEdit->text());
    }
    else
    {
        m_proxyModel->setFilterKeyColumn(WarehouseModel::ColCategory);
        m_proxyModel->setFilterFixedString(category);
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    }
}

void MainWindow::updateStatusBar()
{
    // Статистика / автоматические вычисления (дополнительная возможность)
    QString text = QString("Записей: %1  |  Общее кол-во: %2  |  Общая стоимость: %3 руб.  |  Низкий остаток (≤%4): %5")
                       .arg(m_model->rowCount())
                       .arg(m_model->totalQuantity())
                       .arg(QString::number(m_model->totalValue(), 'f', 2))
                       .arg(WarehouseModel::LOW_STOCK_THRESHOLD)
                       .arg(m_model->lowStockCount(WarehouseModel::LOW_STOCK_THRESHOLD));
    m_statsLabel->setText(text);
}
