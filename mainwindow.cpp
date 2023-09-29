#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <limits>

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "common.h"

//override unordered_map to map to imply order
using value = toml::basic_value<TOML11_DEFAULT_COMMENT_STRATEGY, std::map, std::vector>;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this->ui->actionSave_As, &QAction::triggered, this, &MainWindow::save);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gen_code_name(const QString& new_text){

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the QGridLayout, this is actually the TabWidget)
    QWidget* w = qobject_cast<QWidget*>(s->parent());
    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(w->layout());
    //get the line edit we need from that layout
    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(g->itemAtPosition(0, 3)->widget());

    if (codeNameEdit->isReadOnly()){
        std::string codename_std = generate_code_name(new_text.toUtf8().constData());
        QString cn_qs(codename_std.c_str());
        codeNameEdit->setText(cn_qs);
    }

}

void MainWindow::set_codename_generation(int custom_codename_checked)
{
    //what follows is me pulling the line edit item using just the sender as context:

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the QGridLayout, this is actually the TabWidget)
    QWidget* w = qobject_cast<QWidget*>(s->parent());
    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(w->layout());
    //get the line edit we need from that layout
    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(g->itemAtPosition(0, 3)->widget());

    codeNameEdit->setReadOnly(custom_codename_checked == Qt::Unchecked);
}

void MainWindow::set_reg_block_name(const QString &new_name)
{
    std::string new_name_std(new_name.toUtf8().constData());

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the QGridLayout, this is actually the TabWidget)
    QWidget* w = qobject_cast<QWidget*>(s->parent());

    RegisterBlock* rb = this->reg_blocks.at(w);
    rb->name = new_name_std;
}

void MainWindow::set_reg_block_codename(const QString &new_name)
{
    std::string new_name_std(new_name.toUtf8().constData());

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the QGridLayout, this is actually the TabWidget)
    QWidget* w = qobject_cast<QWidget*>(s->parent());

    RegisterBlock* rb = this->reg_blocks.at(w);
    rb->code_name = new_name_std;
}

void MainWindow::set_reg_block_size(int new_size)
{
    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the QGridLayout, this is actually the TabWidget)
    QWidget* w = qobject_cast<QWidget*>(s->parent());

    RegisterBlock* rb = this->reg_blocks.at(w);
    rb->size = new_size;
}

void MainWindow::save()
{
    QString start_path = QStandardPaths::displayName(QStandardPaths::DesktopLocation);
    QString save_location = QFileDialog::getSaveFileName(this, "Save As...", start_path, "TOML Files (*.toml)");
    printf("Save as: %s\n", save_location.toUtf8().constData());

    QFile save_file(save_location);
    if (!(save_file.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate | QIODeviceBase::Text))){
        QMessageBox::warning(this, "File save Failed", "Failed to save file");
        return;
    }

    QTextStream savefilestream(&save_file);
    std::stringstream std_stream;

    for (std::pair<QWidget*, RegisterBlock*> p : this->reg_blocks){
        RegisterBlock* rb = p.second;
        value rb_table{
            {"name", rb->name.c_str()},
            {"sourcename", rb->code_name.c_str()},
            {"size", rb->size},
            //TODO: add domain offsets
            //TODO: add registers
        };
        std_stream << rb_table << std::endl;
    }

    savefilestream << std_stream.str().c_str();

    save_file.close();
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
    connect(nameEdit, &QLineEdit::textEdited, this, &MainWindow::gen_code_name);
    connect(nameEdit, &QLineEdit::textEdited, this, &MainWindow::set_reg_block_name);
    g->addWidget(nameEdit, currRow, 1);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    g->addWidget(codeNameLabel, currRow, 2);

    QLineEdit* codeNameEdit = new QLineEdit();
    codeNameEdit->setReadOnly(true);
    connect(codeNameEdit, &QLineEdit::textChanged, this, &MainWindow::set_reg_block_codename);
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
    connect(sizeEdit, &QSpinBox::valueChanged, this, &MainWindow::set_reg_block_size);
    g->addWidget(sizeEdit, currRow, 1);

    ++currRow;

    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    g->addItem(spacer, currRow, 0);

    w->setLayout(g);

    RegisterBlock* rb = this->reg_map.add_register_block();
    this->reg_blocks.insert({w, rb});

    //triggers slot which attempt to reference RegisterBlock within reg_blocks map.
    //fails before that record is populated.
    sizeEdit->setValue(1024);
}

