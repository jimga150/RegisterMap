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

void MainWindow::set_codename_generation(int custom_codename_checked)
{
    //what follows is me pulling the line edit item using just the sender as context:

    //get the sending checkbox
    QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
    //get the parent widget (not the QGridLayout, this is actually the TabWidget)
    QWidget* w = qobject_cast<QWidget*>(cb->parent());
    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(w->layout());
    //get the line edit we need from that layout
    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(g->itemAtPosition(0, 3)->widget());

    codeNameEdit->setReadOnly(custom_codename_checked == Qt::Unchecked);
}

void MainWindow::save()
{
    QString start_path = QStandardPaths::displayName(QStandardPaths::DesktopLocation);
    QUrl start_url(start_path);
    QUrl save_location = QFileDialog::getSaveFileUrl(this, "Save As...", start_url, "All Files (*)");
    printf("Save as: %s\n", save_location.path().toUtf8().constData());


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
    connect(customCNCheckBox, &QCheckBox::stateChanged, this, &MainWindow::set_codename_generation);
    g->addWidget(customCNCheckBox, currRow, 3);

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

