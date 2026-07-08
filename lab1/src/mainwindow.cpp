#include "mainwindow.h"
#include <QScrollArea>

#include "laba7.h"
#include "laba10.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    QLabel *menuLabel = new QLabel("Выбор лабораторной:", this);
    QComboBox *labChooser = new QComboBox(this);
    labChooser->addItems({"Лабораторная №1", "Лабораторная №7", "Лабораторная №10"});

    leftLayout->addWidget(menuLabel);
    leftLayout->addWidget(labChooser);
    leftLayout->addStretch();
    mainLayout->addLayout(leftLayout, 1);

    QStackedWidget *stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget, 4);

    QWidget *lab1Page = new QWidget();
    QVBoxLayout *lab1Layout = new QVBoxLayout(lab1Page);

    lab1Layout->addWidget(new QLabel("<h2>Лабораторная №1: Расчет площади</h2>"));

    m_lab1Input = new QLineEdit(this);
    m_lab1Input->setObjectName("l1i");
    m_lab1Input->setPlaceholderText("Введите радиус...");
    lab1Layout->addWidget(m_lab1Input);

    QPushButton *lab1Btn = new QPushButton("Рассчитать", this);
    lab1Btn->setObjectName("l1b");
    lab1Layout->addWidget(lab1Btn);

    m_lab1Result = new QLabel("Результат: ", this);
    m_lab1Result->setObjectName("l1r");
    m_lab1Result->setFixedHeight(30);
    m_lab1Result->setStyleSheet("QLabel { background-color: #383b3b; border: 1px solid #d7dbde; color: #d7dbde; font-size: 14px; }");
    lab1Layout->addWidget(m_lab1Result);
    lab1Layout->addStretch();

    stackedWidget->addWidget(lab1Page);

    QWidget *lab2Page = new QWidget();
    QVBoxLayout *lab2Layout = new QVBoxLayout(lab2Page);

    lab2Layout->addWidget(new QLabel("<h2>Лабораторная №7: Замена максимального и минимального слова</h2>"));

    m_lab2Input = new QLineEdit(this);
    m_lab2Input->setObjectName("l2i");
    m_lab2Input->setPlaceholderText("Введите текст...");
    lab2Layout->addWidget(m_lab2Input);

    QPushButton *lab2Btn = new QPushButton("Заменить слова", this);
    lab2Btn->setObjectName("l2b");
    lab2Layout->addWidget(lab2Btn);
    lab2Layout->addStretch();

    m_lab2Result = new QLabel("Результат: ", this);
    m_lab2Result->setObjectName("l2r");
    m_lab2Result->setFixedHeight(100);
    m_lab2Result->setStyleSheet("QLabel { background-color: #383b3b; border: 1px solid #d7dbde; color: #d7dbde; font-size: 14px; }");
    lab2Layout->addWidget(m_lab2Result);
    lab2Layout->addStretch();

    stackedWidget->addWidget(lab2Page);

    QWidget *lab3Page = new QWidget();
    QVBoxLayout *lab3Layout = new QVBoxLayout(lab3Page);
    lab3Layout->addWidget(new QLabel("<h2>Лабораторная №10: Работа с дробями</h2>"));
    lab3Layout->addStretch();

    m_lab3Input1 = new QLineEdit(this);
    m_lab3Input1->setObjectName("l3i1");
    m_lab3Input1->setPlaceholderText("Введите первую целочисленную дробь (например, 1/2)...");
    lab3Layout->addWidget(m_lab3Input1);

    m_lab3Input2 = new QLineEdit(this);
    m_lab3Input2->setObjectName("l3i2");
    m_lab3Input2->setPlaceholderText("Введите вторую целочисленную дробь (например, 3/4)...");
    lab3Layout->addWidget(m_lab3Input2);

    m_lab3Operation = new QComboBox(this);
    m_lab3Operation->setObjectName("l3op");
    m_lab3Operation->addItem("Сложение (f1 + f2)");
    m_lab3Operation->addItem("Вычитание (f1 - f2)");
    m_lab3Operation->addItem("Умножение (f1 * f2)");
    m_lab3Operation->addItem("Деление (f1 / f2)");
    m_lab3Operation->addItem("Унарный минус (-f1)");
    lab3Layout->addWidget(m_lab3Operation);

    QPushButton *lab3Btn = new QPushButton("Вычислить", this);
    lab3Btn->setObjectName("l3b");
    lab3Layout->addWidget(lab3Btn);

    m_lab3Result = new QLabel("", this);
    m_lab3Result->setObjectName("l3r");
    m_lab3Result->setStyleSheet("QLabel { background-color: #383b3b; border: 1px solid #d7dbde; color: #d7dbde; font-size: 14px; }");
    lab3Layout->addWidget(m_lab3Result);
    lab3Layout->addStretch();

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(m_lab3Result);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedHeight(200);

    lab3Layout->addWidget(scrollArea);

    stackedWidget->addWidget(lab3Page);

    connect(labChooser, &QComboBox::currentIndexChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(lab1Btn, &QPushButton::clicked, this, &MainWindow::onLab1ButtonClicked);
    connect(lab2Btn, &QPushButton::clicked, this, &MainWindow::onLab2ButtonClicked);
    connect(lab3Btn, &QPushButton::clicked, this, &MainWindow::onLab3ButtonClicked);
    resize(750, 400);
    setWindowTitle("Лабораторные работы");
}

MainWindow::~MainWindow()
{
    delete m_lab1Result;
    delete m_lab1Input;
    delete m_lab2Input;
    delete m_lab2Result;
    delete m_lab3Input1;
    delete m_lab3Input2;
    delete m_lab3Operation;
    delete m_lab3Result;
}

void MainWindow::onLab1ButtonClicked()
{
    QString text = m_lab1Input->text();
    bool ok;
    double radius = text.toDouble(&ok);

    if (ok && radius >= 0)
    {
        double area = 3.14159 * radius * radius;
        m_lab1Result->setText(QString("Площадь круга: %1").arg(area));
    }
    else
    {
        m_lab1Result->setText("Ошибка: введите число >= 0");
    }
}

void MainWindow::onLab2ButtonClicked()
{
    QString text = m_lab2Input->text();
    m_lab2Result->setText(QString("Результат: %1").arg(Laba7(text.toStdString().data())));
}

void MainWindow::onLab3ButtonClicked()
{
    QString text1 = m_lab3Input1->text();
    QString text2 = m_lab3Input2->text();
    int operation = m_lab3Operation->currentIndex();
    m_lab3Result->setText(QString("Результат: %1").arg(Laba10(text1, text2, operation)));
}