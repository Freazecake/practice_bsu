#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLab1ButtonClicked();
    void onLab2ButtonClicked();
    void onLab3ButtonClicked();

private:
    QLineEdit *m_lab1Input;
    QLabel *m_lab1Result;

    QLineEdit *m_lab2Input;
    QLabel *m_lab2Result;

    QLineEdit *m_lab3Input1;
    QLineEdit *m_lab3Input2;
    QComboBox *m_lab3Operation;
    QLabel *m_lab3Result;
};