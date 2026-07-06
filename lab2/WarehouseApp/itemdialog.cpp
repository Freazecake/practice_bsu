#include "itemdialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QMessageBox>
#include <QVBoxLayout>

ItemDialog::ItemDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Товар");
    setMinimumWidth(320);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Например, Ноутбук");

    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->setEditable(true);
    m_categoryCombo->addItems({"Электроника", "Мебель", "Канцелярия", "Инструменты", "Продукты", "Прочее"});

    m_quantitySpin = new QSpinBox(this);
    m_quantitySpin->setRange(0, 1000000);

    m_priceSpin = new QDoubleSpinBox(this);
    m_priceSpin->setRange(0.0, 100000000.0);
    m_priceSpin->setDecimals(2);
    m_priceSpin->setSuffix(" руб.");

    auto *form = new QFormLayout;
    form->addRow("Наименование:", m_nameEdit);
    form->addRow("Категория:", m_categoryCombo);
    form->addRow("Количество:", m_quantitySpin);
    form->addRow("Цена:", m_priceSpin);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &ItemDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void ItemDialog::setItem(const WarehouseItem &item)
{
    m_item = item;
    m_nameEdit->setText(item.name);
    m_categoryCombo->setCurrentText(item.category);
    m_quantitySpin->setValue(item.quantity);
    m_priceSpin->setValue(item.price);
}

WarehouseItem ItemDialog::item() const
{
    return m_item;
}

void ItemDialog::onAccept()
{
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка ввода", "Наименование товара не может быть пустым.");
        m_nameEdit->setFocus();
        return;
    }
    if (m_categoryCombo->currentText().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка ввода", "Укажите категорию товара.");
        m_categoryCombo->setFocus();
        return;
    }

    m_item.name = name;
    m_item.category = m_categoryCombo->currentText().trimmed();
    m_item.quantity = m_quantitySpin->value();
    m_item.price = m_priceSpin->value();

    accept();
}
