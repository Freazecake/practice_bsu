#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "productcardstore.h"

class QListWidget;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QDateEdit;
class QComboBox;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddRecord();
    void onApplyChanges();
    void onDeleteRecord();
    void onListSelectionChanged();
    void onSearchTextChanged(const QString &text);
    void onSortFieldChanged();

    void onNewFile();
    void onOpenText();
    void onOpenBinary();
    void onSaveText();
    void onSaveTextAs();
    void onSaveBinary();
    void onSaveBinaryAs();
    void onAbout();

private:
    void setupUi();
    void setupMenu();
    void setupSampleData();
    void refreshList();
    void loadFormFromRecord(const ProductCard &card);
    bool readFormIntoRecord(ProductCard &card, QString *errorMessage);
    void setDirty(bool dirty);
    bool confirmDiscardChangesIfNeeded();
    void restoreSettings();
    void saveSettings();

    ProductCardStore *m_store;

    QListWidget *m_listWidget;
    QLineEdit *m_searchEdit;
    QComboBox *m_sortField;
    QComboBox *m_sortOrder;

    QLineEdit *m_nameEdit;
    QLineEdit *m_categoryEdit;
    QSpinBox *m_quantitySpin;
    QDoubleSpinBox *m_priceSpin;
    QDateEdit *m_receivedDateEdit;
    QLabel *m_idLabel;

    QString m_currentTextPath;
    QString m_currentBinaryPath;
    bool m_dirty = false;
    QVector<int> m_visibleIndices; // индексы записей store, отображаемые в списке (после поиска)
};

#endif // MAINWINDOW_H
