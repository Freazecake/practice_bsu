#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>
#include "warehousemodel.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;

class ItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemDialog(QWidget *parent = nullptr);

    void setItem(const WarehouseItem &item);
    WarehouseItem item() const;

private slots:
    void onAccept();

private:
    QLineEdit *m_nameEdit;
    QComboBox *m_categoryCombo;
    QSpinBox *m_quantitySpin;
    QDoubleSpinBox *m_priceSpin;
    WarehouseItem m_item;
};

#endif // ITEMDIALOG_H
