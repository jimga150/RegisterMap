#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <limits>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_new_reg_block_btn_clicked()
{
    QWidget* w = new QWidget();
    this->ui->tabWidget->addTab(w, "Register Block");

    QGridLayout* g = new QGridLayout();

    int currRow = 0;

    QLabel* nameLabel = new QLabel("Name: ");
    g->addWidget(nameLabel, currRow, 0);

    QLineEdit* nameEdit = new QLineEdit();
    g->addWidget(nameEdit, currRow, 1);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    g->addWidget(codeNameLabel, currRow, 2);

    QLineEdit* codeNameEdit = new QLineEdit();
    g->addWidget(codeNameEdit, currRow, 3);


    ++currRow;

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    g->addWidget(customCNCheckBox, currRow, 3); //TODO: make this enable or disable auto-codename generation

    ++currRow;

    QLabel* sizeLabel = new QLabel("Size (in addrs): ");
    g->addWidget(sizeLabel, currRow, 0);

    QSpinBox* sizeEdit = new QSpinBox();
    sizeEdit->setMinimum(0);
    sizeEdit->setMaximum(std::numeric_limits<int>::max());
    sizeEdit->setValue(1024);
    g->addWidget(sizeEdit, currRow, 1);

    ++currRow;

    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    g->addItem(spacer, currRow, 0);

    w->setLayout(g);
}

