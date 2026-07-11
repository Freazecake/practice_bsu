#include "mainwindow.h"

#include <QMenuBar>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_store = new ProductCardStore(this);

    setupUi();
    setupMenu();
    setupSampleData();
    refreshList();
    restoreSettings();

    setWindowTitle("Лаба 3");
    setDirty(false);
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);

    auto *leftBox = new QVBoxLayout;

    auto *searchLayout = new QHBoxLayout;
    m_searchEdit = new QLineEdit(central);
    m_searchEdit->setPlaceholderText("Поиск по названию, категории или ID...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    searchLayout->addWidget(new QLabel("Поиск:"));
    searchLayout->addWidget(m_searchEdit, 1);

    auto *sortLayout = new QHBoxLayout;
    m_sortField = new QComboBox(central);
    m_sortField->addItem("ID", static_cast<int>(ProductCardStore::SortField::Id));
    m_sortField->addItem("Наименование", static_cast<int>(ProductCardStore::SortField::Name));
    m_sortField->addItem("Категория", static_cast<int>(ProductCardStore::SortField::Category));
    m_sortField->addItem("Количество", static_cast<int>(ProductCardStore::SortField::Quantity));
    m_sortField->addItem("Цена", static_cast<int>(ProductCardStore::SortField::Price));
    m_sortField->addItem("Дата поступления", static_cast<int>(ProductCardStore::SortField::ReceivedDate));

    m_sortOrder = new QComboBox(central);
    m_sortOrder->addItem("по возрастанию", static_cast<int>(Qt::AscendingOrder));
    m_sortOrder->addItem("по убыванию", static_cast<int>(Qt::DescendingOrder));

    connect(m_sortField, &QComboBox::currentIndexChanged, this, &MainWindow::onSortFieldChanged);
    connect(m_sortOrder, &QComboBox::currentIndexChanged, this, &MainWindow::onSortFieldChanged);

    sortLayout->addWidget(new QLabel("Сортировка:"));
    sortLayout->addWidget(m_sortField, 1);
    sortLayout->addWidget(m_sortOrder, 1);

    m_listWidget = new QListWidget(central);
    connect(m_listWidget, &QListWidget::currentRowChanged, this, &MainWindow::onListSelectionChanged);

    auto *listButtons = new QHBoxLayout;
    auto *addBtn = new QPushButton("Добавить", central);
    auto *delBtn = new QPushButton("Удалить", central);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddRecord);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteRecord);
    listButtons->addWidget(addBtn);
    listButtons->addWidget(delBtn);

    leftBox->addLayout(searchLayout);
    leftBox->addLayout(sortLayout);
    leftBox->addWidget(m_listWidget, 1);
    leftBox->addLayout(listButtons);

    auto *formBox = new QGroupBox("Карточка товара", central);
    auto *form = new QFormLayout(formBox);

    m_idLabel = new QLabel("—", formBox);
    m_nameEdit = new QLineEdit(formBox);
    m_categoryEdit = new QLineEdit(formBox);
    m_quantitySpin = new QSpinBox(formBox);
    m_quantitySpin->setRange(0, 1000000);
    m_priceSpin = new QDoubleSpinBox(formBox);
    m_priceSpin->setRange(0.0, 100000000.0);
    m_priceSpin->setDecimals(2);
    m_priceSpin->setSuffix(" руб.");
    m_receivedDateEdit = new QDateEdit(QDate::currentDate(), formBox);
    m_receivedDateEdit->setCalendarPopup(true);
    m_receivedDateEdit->setDisplayFormat("dd.MM.yyyy");

    form->addRow("ID:", m_idLabel);
    form->addRow("Наименование:", m_nameEdit);
    form->addRow("Категория:", m_categoryEdit);
    form->addRow("Количество:", m_quantitySpin);
    form->addRow("Цена:", m_priceSpin);
    form->addRow("Дата поступления:", m_receivedDateEdit);

    auto *applyBtn = new QPushButton("Применить изменения", formBox);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::onApplyChanges);
    form->addRow(applyBtn);

    mainLayout->addLayout(leftBox, 2);
    mainLayout->addWidget(formBox, 3);

    setCentralWidget(central);
    resize(920, 560);
    statusBar()->showMessage("Готово");
}

void MainWindow::setupMenu()
{
    auto *fileMenu = menuBar()->addMenu("&Файл");
    fileMenu->addAction("Создать", QKeySequence::New, this, &MainWindow::onNewFile);
    fileMenu->addSeparator();
    fileMenu->addAction("Открыть (текстовый формат)...", this, &MainWindow::onOpenText);
    fileMenu->addAction("Открыть (бинарный формат)...", QKeySequence::Open, this, &MainWindow::onOpenBinary);
    fileMenu->addSeparator();
    fileMenu->addAction("Сохранить (текстовый)", QKeySequence::Save, this, &MainWindow::onSaveText);
    fileMenu->addAction("Сохранить как (текстовый)...", this, &MainWindow::onSaveTextAs);
    fileMenu->addAction("Сохранить (бинарный)", this, &MainWindow::onSaveBinary);
    fileMenu->addAction("Сохранить как (бинарный)...", this, &MainWindow::onSaveBinaryAs);
    fileMenu->addSeparator();
    fileMenu->addAction("Выход", QKeySequence::Quit, this, &QWidget::close);

    auto *editMenu = menuBar()->addMenu("&Правка");
    editMenu->addAction("Добавить запись", this, &MainWindow::onAddRecord);
    editMenu->addAction("Удалить запись", this, &MainWindow::onDeleteRecord);
    editMenu->addSeparator();
    editMenu->addAction("Найти...", QKeySequence::Find, this, [this]()
                        { m_searchEdit->setFocus(); m_searchEdit->selectAll(); });

    auto *helpMenu = menuBar()->addMenu("&Справка");
    helpMenu->addAction("О программе", this, &MainWindow::onAbout);
}

void MainWindow::setupSampleData()
{
    m_store->addRecord({m_store->nextId(), "Ноутбук", "Электроника", 15, 450.00, QDate(2026, 3, 10)});
    m_store->addRecord({m_store->nextId(), "Стол офисный", "Мебель", 10, 800.00, QDate(2026, 4, 2)});
    m_store->addRecord({m_store->nextId(), "Принтер лазерный", "Электроника", 8, 1200.00, QDate(2026, 5, 20)});
}

void MainWindow::refreshList()
{
    QString query = m_searchEdit ? m_searchEdit->text() : QString();
    m_visibleIndices = m_store->findByText(query);

    int previousRow = m_listWidget->currentRow();

    m_listWidget->blockSignals(true);
    m_listWidget->clear();
    for (int idx : m_visibleIndices)
    {
        ProductCard c = m_store->at(idx);
        m_listWidget->addItem(QString("#%1 — %2 (%3)").arg(c.id).arg(c.name, c.category));
    }
    m_listWidget->blockSignals(false);

    if (previousRow >= 0 && previousRow < m_listWidget->count())
        m_listWidget->setCurrentRow(previousRow);
    else if (m_listWidget->count() > 0)
        m_listWidget->setCurrentRow(0);
    else
        loadFormFromRecord(ProductCard());
}

void MainWindow::loadFormFromRecord(const ProductCard &card)
{
    m_idLabel->setText(card.id > 0 ? QString::number(card.id) : "—");
    m_nameEdit->setText(card.name);
    m_categoryEdit->setText(card.category);
    m_quantitySpin->setValue(card.quantity);
    m_priceSpin->setValue(card.price);
    m_receivedDateEdit->setDate(card.receivedDate.isValid() ? card.receivedDate : QDate::currentDate());
}

bool MainWindow::readFormIntoRecord(ProductCard &card, QString *errorMessage)
{
    card.name = m_nameEdit->text().trimmed();
    card.category = m_categoryEdit->text().trimmed();
    card.quantity = m_quantitySpin->value();
    card.price = m_priceSpin->value();
    card.receivedDate = m_receivedDateEdit->date();

    return ProductCardStore::validate(card, errorMessage);
}

void MainWindow::onListSelectionChanged()
{
    int row = m_listWidget->currentRow();
    if (row < 0 || row >= m_visibleIndices.size())
    {
        loadFormFromRecord(ProductCard());
        return;
    }
    loadFormFromRecord(m_store->at(m_visibleIndices.at(row)));
}

void MainWindow::onAddRecord()
{
    ProductCard card;
    card.id = m_store->nextId();
    card.name = "Новый товар";
    card.category = "Без категории";
    card.receivedDate = QDate::currentDate();

    m_store->addRecord(card);
    setDirty(true);
    refreshList();
    m_listWidget->setCurrentRow(m_listWidget->count() - 1);
    statusBar()->showMessage("Запись добавлена", 3000);
}

void MainWindow::onApplyChanges()
{
    int row = m_listWidget->currentRow();
    if (row < 0 || row >= m_visibleIndices.size())
    {
        QMessageBox::information(this, "Применить изменения", "Выберите запись в списке.");
        return;
    }

    ProductCard card;
    QString error;
    if (!readFormIntoRecord(card, &error))
    {
        QMessageBox::warning(this, "Ошибка ввода", error);
        return;
    }

    m_store->updateRecord(m_visibleIndices.at(row), card);
    setDirty(true);
    refreshList();
    statusBar()->showMessage("Изменения применены", 3000);
}

void MainWindow::onDeleteRecord()
{
    int row = m_listWidget->currentRow();
    if (row < 0 || row >= m_visibleIndices.size())
    {
        QMessageBox::information(this, "Удаление", "Выберите запись для удаления.");
        return;
    }

    ProductCard card = m_store->at(m_visibleIndices.at(row));
    auto reply = QMessageBox::question(this, "Подтверждение удаления",
                                       QString("Удалить запись \"%1\"?").arg(card.name),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    m_store->removeRecord(m_visibleIndices.at(row));
    setDirty(true);
    refreshList();
    statusBar()->showMessage("Запись удалена", 3000);
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text);
    refreshList();
}

void MainWindow::onSortFieldChanged()
{
    auto field = static_cast<ProductCardStore::SortField>(m_sortField->currentData().toInt());
    auto order = static_cast<Qt::SortOrder>(m_sortOrder->currentData().toInt());
    m_store->sortBy(field, order);
    setDirty(true);
    refreshList();
}

bool MainWindow::confirmDiscardChangesIfNeeded()
{
    if (!m_dirty)
        return true;

    auto reply = QMessageBox::question(
        this, "Несохранённые изменения",
        "Есть несохранённые изменения. Сохранить перед продолжением?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel)
        return false;
    if (reply == QMessageBox::Save)
    {
        if (!m_currentTextPath.isEmpty())
            onSaveText();
        else
            onSaveTextAs();
    }
    return true;
}

void MainWindow::onNewFile()
{
    if (!confirmDiscardChangesIfNeeded())
        return;

    m_store->clear();
    m_currentTextPath.clear();
    m_currentBinaryPath.clear();
    refreshList();
    setWindowTitle("Карточка товара — новый файл");
    setDirty(false);
}

void MainWindow::onOpenText()
{
    if (!confirmDiscardChangesIfNeeded())
        return;

    QString path = QFileDialog::getOpenFileName(this, "Открыть текстовый файл", QString(), "Текстовые файлы (*.txt)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_store->loadText(path, &error))
    {
        QMessageBox::critical(this, "Ошибка загрузки", error);
        return;
    }

    m_currentTextPath = path;
    refreshList();
    setWindowTitle("Карточка товара — " + path);
    setDirty(false);
    statusBar()->showMessage("Файл загружен: " + path, 3000);
}

void MainWindow::onOpenBinary()
{
    if (!confirmDiscardChangesIfNeeded())
        return;

    QString path = QFileDialog::getOpenFileName(this, "Открыть бинарный файл", QString(), "Бинарные файлы (*.dat)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_store->loadBinary(path, &error))
    {
        QMessageBox::critical(this, "Ошибка загрузки", error);
        return;
    }

    m_currentBinaryPath = path;
    refreshList();
    setWindowTitle("Карточка товара — " + path);
    setDirty(false);
    statusBar()->showMessage("Файл загружен: " + path, 3000);
}

void MainWindow::onSaveText()
{
    if (m_currentTextPath.isEmpty())
    {
        onSaveTextAs();
        return;
    }
    QString error;
    if (!m_store->saveText(m_currentTextPath, &error))
        QMessageBox::critical(this, "Ошибка сохранения", error);
    else
    {
        setDirty(false);
        statusBar()->showMessage("Сохранено: " + m_currentTextPath, 3000);
    }
}

void MainWindow::onSaveTextAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить как (текстовый)", "products.txt", "Текстовые файлы (*.txt)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_store->saveText(path, &error))
    {
        QMessageBox::critical(this, "Ошибка сохранения", error);
        return;
    }
    m_currentTextPath = path;
    setWindowTitle("Карточка товара — " + path);
    setDirty(false);
    statusBar()->showMessage("Сохранено: " + path, 3000);
}

void MainWindow::onSaveBinary()
{
    if (m_currentBinaryPath.isEmpty())
    {
        onSaveBinaryAs();
        return;
    }
    QString error;
    if (!m_store->saveBinary(m_currentBinaryPath, &error))
        QMessageBox::critical(this, "Ошибка сохранения", error);
    else
    {
        setDirty(false);
        statusBar()->showMessage("Сохранено: " + m_currentBinaryPath, 3000);
    }
}

void MainWindow::onSaveBinaryAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить как (бинарный)", "products.dat", "Бинарные файлы (*.dat)");
    if (path.isEmpty())
        return;

    QString error;
    if (!m_store->saveBinary(path, &error))
    {
        QMessageBox::critical(this, "Ошибка сохранения", error);
        return;
    }
    m_currentBinaryPath = path;
    setWindowTitle("Карточка товара — " + path);
    setDirty(false);
    statusBar()->showMessage("Сохранено: " + path, 3000);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "О программе",
                       "Лаба 3 - «Карточка товара»\n");
}

void MainWindow::setDirty(bool dirty)
{
    m_dirty = dirty;
    QString title = windowTitle();
    if (dirty && !title.endsWith(" *"))
        setWindowTitle(title + " *");
    else if (!dirty && title.endsWith(" *"))
        setWindowTitle(title.chopped(2));
}

void MainWindow::restoreSettings()
{
    QSettings settings("QtLab3", "ProductCardApp");
    if (settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());
    m_currentTextPath = settings.value("lastTextPath").toString();
    m_currentBinaryPath = settings.value("lastBinaryPath").toString();
}

void MainWindow::saveSettings()
{
    QSettings settings("QtLab3", "ProductCardApp");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("lastTextPath", m_currentTextPath);
    settings.setValue("lastBinaryPath", m_currentBinaryPath);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!confirmDiscardChangesIfNeeded())
    {
        event->ignore();
        return;
    }
    saveSettings();
    event->accept();
}
