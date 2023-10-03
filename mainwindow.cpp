#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <limits>

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "common.h"

#define REG_BLOCK_FIELD_COORD_NAME          (std::pair<int, int>(0, 1))
#define REG_BLOCK_FIELD_COORD_CODENAME      (std::pair<int, int>(0, 3))
#define REG_BLOCK_FIELD_COORD_GEN_CODENAME  (std::pair<int, int>(1, 3))
#define REG_BLOCK_FIELD_COORD_SIZE          (std::pair<int, int>(2, 1))
#define REG_BLOCK_FIELD_COORD_DESC_LABEL    (std::pair<int, int>(3, 0))
#define REG_BLOCK_FIELD_COORD_DESC          (std::pair<int, int>(4, 0))
#define REG_BLOCK_FIELD_COORD_REGTABLE      (std::pair<int, int>(5, 0))
#define REG_BLOCK_FIELD_COORD_NEWREGBTN     (std::pair<int, int>(6, 0))
#define REG_BLOCK_FIELD_COORD_REGFRAME      (std::pair<int, int>(7, 0))
#define REG_BLOCK_FIELD_COORD_SPACER        (std::pair<int, int>(8, 0))

#define REG_FIELD_COORD_NAME            (std::pair<int, int>(0, 1))
#define REG_FIELD_COORD_CODENAME        (std::pair<int, int>(0, 3))
#define REG_FIELD_COORD_GEN_CODENAME    (std::pair<int, int>(1, 3))
#define REG_FIELD_COORD_OFFSET          (std::pair<int, int>(2, 1))
#define REG_FIELD_COORD_DESC_LABEL      (std::pair<int, int>(3, 0))
#define REG_FIELD_COORD_DESC            (std::pair<int, int>(4, 0))

#define REG_TABLE_COL_NAME      (0)
#define REG_TABLE_COL_OFFSET    (1)
#define REG_TABLE_COL_DESC      (2)
#define REG_TABLE_COL_MAX       REG_TABLE_COL_DESC

//override unordered_map to map to imply order
using value = toml::basic_value<TOML11_DEFAULT_COMMENT_STRATEGY, std::map, std::vector>;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this->ui->actionSave_As, &QAction::triggered, this, &MainWindow::save);

    this->codename_validator = new QRegularExpressionValidator(QRegularExpression("[0-9][A-Z][a-z]_"), this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    value base_table{{"version", 0.1}};

    std::string rbid;
    int reg_blk_id_num = 0;

    for (RegisterBlockController* p : this->reg_block_ctrls){

        value reg_array;

        std::string rid;

        for (int i = 0; i < p->getNumRegs(); ++i){
            value reg_record{
                {"name", p->getRegName(i).toStdString()},
                {"sourcename", p->getRegCodeName(i).toStdString()},
                {"offset", p->getRegOffset(i)},
            };

            rid = "reg";
            rid += QString::number(i).toUtf8().constData();
            reg_array[rid.c_str()] = reg_record;
        }

        value rb_table{
            {"name", p->getName().toStdString()},
            {"sourcename", p->getCodeName().toStdString()},
            {"size", p->getSize()},
            {"registers", reg_array}
        };

        rbid = "regblk";
        rbid += QString::number(reg_blk_id_num++).toUtf8().constData();
        base_table[rbid.c_str()] = rb_table;
    }

    std_stream << base_table << std::endl;

    savefilestream << std_stream.str().c_str();

    save_file.close();
}


void MainWindow::on_new_reg_block_btn_clicked()
{
    QWidget* w = new QWidget();
    this->ui->tabWidget->addTab(w, "Register Block");

    RegisterBlockController* rbc = new RegisterBlockController(w);
    this->reg_block_ctrls.push_back(rbc);

    QGridLayout* g = new QGridLayout();
    w->setLayout(g);

    QLabel* nameLabel = new QLabel("Name: ");
    g->addWidget(nameLabel, REG_BLOCK_FIELD_COORD_NAME.first, REG_BLOCK_FIELD_COORD_NAME.second-1);

    QLineEdit* nameEdit = new QLineEdit();
    connect(nameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setName);
    connect(rbc, &RegisterBlockController::nameChanged, nameEdit, [=](const QString& newname){
        if (newname != nameEdit->text()){
            nameEdit->setText(newname);
        }
    });
    g->addWidget(nameEdit, REG_BLOCK_FIELD_COORD_NAME.first, REG_BLOCK_FIELD_COORD_NAME.second);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    g->addWidget(codeNameLabel, REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second-1);

    QLineEdit* codeNameEdit = new QLineEdit();
    codeNameEdit->setReadOnly(true);
    codeNameEdit->setValidator(this->codename_validator);
    connect(codeNameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setCodeName);
    connect(rbc, &RegisterBlockController::codeNameChanged, codeNameEdit, [=](const QString& newname){
        if (newname != codeNameEdit->text()){
            codeNameEdit->setText(newname);
        }
    });
    g->addWidget(codeNameEdit, REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second);

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    //WARNING: this relies on the fact that the enum describing the "checked" states of a checkbox place "unchecked" as the 0 value,
    //and other levels of being checked at nonzero values, which will map to the bool type as checked -> true, unchecked -> false.
    //if that enum changes, this breaks. I dont feel like making this super robust though cause i dont want to include Qt GUI stuff in my controller.
    //wah wah.
    connect(customCNCheckBox, &QCheckBox::stateChanged, rbc, &RegisterBlockController::setCodeNameGeneration);
    g->addWidget(customCNCheckBox, REG_BLOCK_FIELD_COORD_GEN_CODENAME.first, REG_BLOCK_FIELD_COORD_GEN_CODENAME.second);

    QLabel* sizeLabel = new QLabel("Size (in addrs): ");
    g->addWidget(sizeLabel, REG_BLOCK_FIELD_COORD_SIZE.first, REG_BLOCK_FIELD_COORD_SIZE.second-1);

    QSpinBox* sizeEdit = new QSpinBox();
    connect(sizeEdit, &QSpinBox::valueChanged, rbc, &RegisterBlockController::setSize);
    connect(rbc, &RegisterBlockController::sizeChanged, sizeEdit, [=](uint32_t new_size){
        if ((int)new_size != sizeEdit->value()){
            sizeEdit->setValue(new_size);
        }
    });
    sizeEdit->setMinimum(0);
    sizeEdit->setMaximum(std::numeric_limits<int>::max());
    g->addWidget(sizeEdit, REG_BLOCK_FIELD_COORD_SIZE.first, REG_BLOCK_FIELD_COORD_SIZE.second);

    //must call this after the spinbox has been added to the gridlayout and the registerblock has been associated with the widget
    sizeEdit->setValue(1024);

    QLabel* descLabel = new QLabel("Decription: ");
    g->addWidget(descLabel, REG_BLOCK_FIELD_COORD_DESC_LABEL.first, REG_BLOCK_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    g->addWidget(descEdit, REG_BLOCK_FIELD_COORD_DESC.first, REG_BLOCK_FIELD_COORD_DESC.second, 1, 4);

    QTableWidget* regTable = new QTableWidget(0, 3);
    regTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    regTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    regTable->setSelectionMode(QAbstractItemView::SingleSelection);
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_OFFSET, new QTableWidgetItem("Offset"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_DESC, new QTableWidgetItem("Descwiption"));
    connect(regTable, &QTableWidget::itemDoubleClicked, rbc, [=](QTableWidgetItem* item){
        rbc->setCurrRegIdx(regTable->row(item));
    });
    connect(rbc, &RegisterBlockController::currRegIdxChanged, regTable, &QTableWidget::selectRow);
    g->addWidget(regTable, REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second, 1, 4);

    QPushButton* newregButton = new QPushButton("New Register");
    connect(newregButton, &QPushButton::clicked, rbc, &RegisterBlockController::makeNewReg);
    g->addWidget(newregButton, REG_BLOCK_FIELD_COORD_NEWREGBTN.first, REG_BLOCK_FIELD_COORD_NEWREGBTN.second);

    QFrame* regFrame = new QFrame();
    regFrame->setLineWidth(5);

    //connect up new register functionality here so we can reference regFrame
    connect(rbc, &RegisterBlockController::regCreated, regTable, [=]
            (const QString& name, uint32_t offset, const QString& description){

        regTable->setRowCount(regTable->rowCount()+1);
        int curr_table_row = regTable->rowCount() - 1;

        regTable->setItem(curr_table_row, REG_TABLE_COL_NAME, new QTableWidgetItem(name));
        regTable->setItem(curr_table_row, REG_TABLE_COL_OFFSET, new QTableWidgetItem(QString::number(offset)));
        regTable->setItem(curr_table_row, REG_TABLE_COL_DESC, new QTableWidgetItem(description));

        rbc->setCurrRegIdx(curr_table_row);

        //loop through children of register frame
        for (QObject* const qo : regFrame->children()){
            //if its a widget (invalid casts return nullptr)
            if (QWidget* const w = qobject_cast<QWidget*>(qo)){
                //enable it
                w->setEnabled(true);
            }
        }
    });

    QGridLayout* reggrid = new QGridLayout();
    regFrame->setLayout(reggrid);

    {
        QLabel* regNameLabel = new QLabel("Name: ");
        regNameLabel->setEnabled(false);
        reggrid->addWidget(regNameLabel, REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second-1);

        QLineEdit* nameEdit = new QLineEdit();
        connect(nameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setRegName);
        connect(rbc, &RegisterBlockController::regNameChanged, nameEdit, &QLineEdit::setText);
        nameEdit->setEnabled(false); //will set editable when register is tracked with this UI
        reggrid->addWidget(nameEdit, REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second);

        QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
        codeNameLabel->setEnabled(false);
        reggrid->addWidget(codeNameLabel, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second-1);

        QLineEdit* codeNameEdit = new QLineEdit();
        codeNameEdit->setEnabled(false);
        codeNameEdit->setReadOnly(true);
        codeNameEdit->setValidator(this->codename_validator);
        connect(codeNameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setRegCodeName);
        connect(rbc, &RegisterBlockController::regCodeNameChanged, codeNameEdit, &QLineEdit::setText);
        reggrid->addWidget(codeNameEdit, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second);

        QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
        connect(customCNCheckBox, &QCheckBox::stateChanged, rbc, &RegisterBlockController::setCodeNameGeneration);
        connect(rbc, &RegisterBlockController::codeNameGenerationChanged, customCNCheckBox, &QCheckBox::setChecked);
        customCNCheckBox->setEnabled(false); //will set editable when register is tracked with this UI
        reggrid->addWidget(customCNCheckBox, REG_FIELD_COORD_GEN_CODENAME.first, REG_FIELD_COORD_GEN_CODENAME.second);

        QLabel* offsetLabel = new QLabel("Offset (in addrs): ");
        offsetLabel->setEnabled(false);
        reggrid->addWidget(offsetLabel, REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second-1);

        QSpinBox* offsetEdit = new QSpinBox();
        connect(offsetEdit, &QSpinBox::valueChanged, rbc, &RegisterBlockController::setRegOffset);
        connect(rbc, &RegisterBlockController::regOffsetChanged, offsetEdit, [=](uint32_t new_offset){
            if (new_offset != (uint32_t)offsetEdit->value()){
                offsetEdit->setValue(new_offset);
            }
        });
        offsetEdit->setMinimum(0);
        offsetEdit->setMaximum(std::numeric_limits<int>::max());
        offsetEdit->setDisplayIntegerBase(16);
        offsetEdit->setPrefix("0x");
        offsetEdit->setEnabled(false); //will set editable when register is tracked with this UI
        reggrid->addWidget(offsetEdit, REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second);

    }

    g->addWidget(regFrame, REG_BLOCK_FIELD_COORD_REGFRAME.first, REG_BLOCK_FIELD_COORD_REGFRAME.second, 1, 4);

    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    g->addItem(spacer, REG_BLOCK_FIELD_COORD_SPACER.first, REG_BLOCK_FIELD_COORD_SPACER.second);

}

