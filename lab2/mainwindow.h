#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTableView;
class QSortFilterProxyModel;
class QLineEdit;
class QComboBox;
class QLabel;
class WarehouseModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddItem();
    void onEditItem();
    void onDeleteItem();
    void onSaveTxt();
    void onLoadTxt();
    void onSaveCsv();
    void onLoadCsv();
    void onSearchTextChanged(const QString &text);
    void onCategoryFilterChanged(int index);
    void updateStatusBar();

private:
    void setupUi();
    void setupSampleData();
    void refreshCategoryFilter();
    int currentSourceRow() const;

    WarehouseModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    QTableView *m_tableView;
    QLineEdit *m_searchEdit;
    QComboBox *m_categoryFilter;
    QLabel *m_statsLabel;
};

#endif // MAINWINDOW_H
