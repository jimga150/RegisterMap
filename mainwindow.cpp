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

void MainWindow::gen_regblk_code_name(const QString& new_text){

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* tab = qobject_cast<QWidget*>(s->parent());
    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(tab->layout());
    //get the line edit we need from that layout
    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second)->widget()
        );

    if (codeNameEdit->isReadOnly()){
        std::string codename_std = generate_code_name(new_text.toUtf8().constData());
        QString cn_qs(codename_std.c_str());
        codeNameEdit->setText(cn_qs);
    }

}

void MainWindow::set_regblk_codename_generation(int custom_codename_checked)
{
    //what follows is me pulling the line edit item using just the sender as context:

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* tab = qobject_cast<QWidget*>(s->parent());
    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(tab->layout());
    //get the line edit we need from that layout
    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second)->widget()
        );

    codeNameEdit->setReadOnly(custom_codename_checked == Qt::Unchecked);

    if (codeNameEdit->isReadOnly()){
        QLineEdit* nameEdit = qobject_cast<QLineEdit*>(
            g->itemAtPosition(REG_BLOCK_FIELD_COORD_NAME.first, REG_BLOCK_FIELD_COORD_NAME.second)->widget()
            );
        QString name = nameEdit->text();
        std::string codename_std = generate_code_name(name.toUtf8().constData());
        QString cn_qs(codename_std.c_str());
        codeNameEdit->setText(cn_qs);
    }
}

void MainWindow::make_new_reg()
{
    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* tab = qobject_cast<QWidget*>(s->parent());
    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(tab->layout());

    RegisterBlock* rb;
    try {
        rb = this->reg_blocks.at(tab);
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
        return;
    }

    QTableWidget* regTable = qobject_cast<QTableWidget*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second)->widget()
        );

    Register reg;
    reg.name = "New Register";
    reg.offset = rb->registers.size() ? rb->registers.at(rb->registers.size()-1).offset + 1 : 0;
    reg.code_name = generate_code_name(reg.name);
    reg.bit_len = 8; //TODO: set default for this in a menu? default per-block?
    reg.description = "Reserved";

    regTable->setRowCount(regTable->rowCount()+1);
    int curr_table_row = regTable->rowCount() - 1;
    regTable->setItem(curr_table_row, REG_TABLE_COL_NAME, new QTableWidgetItem(reg.name.c_str()));
    regTable->setItem(curr_table_row, REG_TABLE_COL_OFFSET, new QTableWidgetItem(QString::number(reg.offset)));
    regTable->setItem(curr_table_row, REG_TABLE_COL_DESC, new QTableWidgetItem(reg.description.c_str()));

    rb->registers.push_back(reg);
}

void MainWindow::set_reg_block_name(const QString &new_name)
{
    std::string new_name_std(new_name.toUtf8().constData());

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* tab = qobject_cast<QWidget*>(s->parent());

    try {
        RegisterBlock* rb = this->reg_blocks.at(tab);
        rb->name = new_name_std;
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
    }
}

void MainWindow::set_reg_block_codename(const QString &new_name)
{
    std::string new_name_std(new_name.toUtf8().constData());

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* tab = qobject_cast<QWidget*>(s->parent());

    try {
        RegisterBlock* rb = this->reg_blocks.at(tab);
        rb->code_name = new_name_std;
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
    }

}

void MainWindow::set_reg_block_size(int new_size)
{
    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* tab = qobject_cast<QWidget*>(s->parent());

    try {
        RegisterBlock* rb = this->reg_blocks.at(tab);
        rb->size = new_size;
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
    }
}

void MainWindow::set_current_reg(QTableWidgetItem* item){

    QTableWidget* regTable = item->tableWidget();
    QWidget* tab = regTable->parentWidget();

    //get the layout from the parent tab widget
    QGridLayout* g = qobject_cast<QGridLayout*>(tab->layout());

    RegisterBlock* rb;
    try {
        rb = this->reg_blocks.at(tab);
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
        return;
    }

    this->current_reg_row = regTable->row(item);
    Register* curr_reg = &(rb->registers.at(this->current_reg_row));

    QFrame* regFrame = qobject_cast<QFrame*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_REGFRAME.first, REG_BLOCK_FIELD_COORD_REGFRAME.second)->widget()
        );

    for (QObject* child : regFrame->children()){
        QWidget* w = qobject_cast<QWidget*>(child);
        if (w) w->setEnabled(true);
    }

    QGridLayout* regGrid = qobject_cast<QGridLayout*>(regFrame->layout());

    QLineEdit* nameEdit = qobject_cast<QLineEdit*>(
        regGrid->itemAtPosition(REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second)->widget()
        );
    nameEdit->setText(curr_reg->name.c_str());

    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(
        regGrid->itemAtPosition(REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second)->widget()
        );
    codeNameEdit->setText(curr_reg->code_name.c_str());

    QSpinBox* offsetSpinBox = qobject_cast<QSpinBox*>(
        regGrid->itemAtPosition(REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second)->widget()
        );
    offsetSpinBox->setValue(curr_reg->offset);

    regTable->setRangeSelected(QTableWidgetSelectionRange(0, 0, regTable->rowCount()-1, regTable->columnCount()-1), false);
    regTable->setRangeSelected(QTableWidgetSelectionRange(this->current_reg_row, 0, this->current_reg_row, regTable->columnCount()-1), true);

}

void MainWindow::set_reg_name(const QString& new_name)
{
    std::string new_name_std = new_name.toUtf8().constData();

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* frame = qobject_cast<QWidget*>(s->parent());
    QWidget* w = qobject_cast<QWidget*>(frame->parent());

    QGridLayout* g = qobject_cast<QGridLayout*>(w->layout());
    QTableWidget* regTable = qobject_cast<QTableWidget*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second)->widget()
        );
    QGridLayout* reggrid = qobject_cast<QGridLayout*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_REGFRAME.first, REG_BLOCK_FIELD_COORD_REGFRAME.second)->widget()->layout()
        );

    RegisterBlock* rb;

    try {
        rb = this->reg_blocks.at(w);
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
        return;
    }

    if (rb->registers.size() == 0){
        return;
    }

    Register* curr_reg = &(rb->registers.at(this->current_reg_row));

    curr_reg->name = new_name_std;

    QLineEdit* cn_edit = qobject_cast<QLineEdit*>(
        reggrid->itemAtPosition(REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second)->widget()
        );
    if (cn_edit->isReadOnly()){
        curr_reg->code_name = generate_code_name(new_name_std);
        cn_edit->setText(curr_reg->code_name.c_str());
    }

    regTable->item(this->current_reg_row, REG_TABLE_COL_NAME)->setText(new_name);
}

void MainWindow::set_reg_codename(const QString& new_codename)
{
    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* frame = qobject_cast<QWidget*>(s->parent());
    QWidget* w = qobject_cast<QWidget*>(frame->parent());

    RegisterBlock* rb;

    try {
        rb = this->reg_blocks.at(w);
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s\n", __FILE__, __LINE__, e.what());
        return;
    }

    std::string new_name_std = new_codename.toUtf8().constData();
    Register* curr_reg = &(rb->registers.at(this->current_reg_row));
    curr_reg->code_name = new_name_std;
}

void MainWindow::set_reg_codename_gen(int custom_codename_checked)
{

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* frame = qobject_cast<QWidget*>(s->parent());
    QWidget* w = qobject_cast<QWidget*>(frame->parent());

    QGridLayout* g = qobject_cast<QGridLayout*>(w->layout());
    QGridLayout* reggrid = qobject_cast<QGridLayout*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_REGFRAME.first, REG_BLOCK_FIELD_COORD_REGFRAME.second)->widget()->layout()
        );

    QLineEdit* codeNameEdit = qobject_cast<QLineEdit*>(
        reggrid->itemAtPosition(REG_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second)->widget()
    );

    codeNameEdit->setReadOnly(custom_codename_checked == Qt::Unchecked);

    if (codeNameEdit->isReadOnly()){
        QLineEdit* nameEdit = qobject_cast<QLineEdit*>(
            reggrid->itemAtPosition(REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second)->widget()
            );
        QString name = nameEdit->text();
        std::string codename_std = generate_code_name(name.toUtf8().constData());
        QString cn_qs(codename_std.c_str());
        codeNameEdit->setText(cn_qs);
    }
}

void MainWindow::set_reg_offset(int new_offset)
{

    //get the sending object
    QObject* s = sender();
    //get the parent widget (not the containing layout,
    //this is actually the widget that owns the layout containing the sender)
    QWidget* frame = qobject_cast<QWidget*>(s->parent());
    QWidget* w = qobject_cast<QWidget*>(frame->parent());

    QGridLayout* g = qobject_cast<QGridLayout*>(w->layout());
    QTableWidget* regTable = qobject_cast<QTableWidget*>(
        g->itemAtPosition(REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second)->widget()
        );

    RegisterBlock* rb;

    try {
        rb = this->reg_blocks.at(w);
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
        return;
    }

    if (rb->registers.size() == 0){
        return;
    }

    Register* curr_reg = &(rb->registers.at(this->current_reg_row));

    curr_reg->offset = new_offset;

    regTable->item(this->current_reg_row, REG_TABLE_COL_OFFSET)->setText(QString::number(new_offset));
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

    for (std::pair<QWidget*, RegisterBlock*> p : this->reg_blocks){
        RegisterBlock* rb = p.second;

        value reg_array;

        std::string rid;
        int reg_id_num = 0;

        for (Register& reg : rb->registers){
            value reg_record{
                             {"name", reg.name.c_str()},
                             {"sourcename", reg.code_name.c_str()},
                             {"offset", reg.offset},
                             };

            rid = "reg";
            rid += (char)(reg_id_num + '0');
            reg_array[rid.c_str()] = reg_record;
            ++reg_id_num;
        }

        value rb_table{
            {"name", rb->name.c_str()},
            {"sourcename", rb->code_name.c_str()},
            {"size", rb->size},
            {"registers", reg_array}
        };

        rbid = "regblk";
        rbid += (char)(reg_blk_id_num + '0');
        base_table[rbid.c_str()] = rb_table;
        ++reg_blk_id_num;
    }

    std_stream << base_table << std::endl;

    savefilestream << std_stream.str().c_str();

    save_file.close();
}


void MainWindow::on_new_reg_block_btn_clicked()
{
    QWidget* w = new QWidget();
    this->ui->tabWidget->addTab(w, "Register Block");

    RegisterBlock* rb = this->reg_map.add_register_block();
    this->reg_blocks.insert({w, rb});

    QGridLayout* g = new QGridLayout();
    w->setLayout(g);

    QLabel* nameLabel = new QLabel("Name: ");
    g->addWidget(nameLabel, REG_BLOCK_FIELD_COORD_NAME.first, REG_BLOCK_FIELD_COORD_NAME.second-1);

    QLineEdit* nameEdit = new QLineEdit();
    connect(nameEdit, &QLineEdit::textEdited, this, &MainWindow::gen_regblk_code_name);
    connect(nameEdit, &QLineEdit::textEdited, this, &MainWindow::set_reg_block_name);
    g->addWidget(nameEdit, REG_BLOCK_FIELD_COORD_NAME.first, REG_BLOCK_FIELD_COORD_NAME.second);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    g->addWidget(codeNameLabel, REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second-1);

    QLineEdit* codeNameEdit = new QLineEdit();
    codeNameEdit->setReadOnly(true);
    codeNameEdit->setValidator(this->codename_validator);
    connect(codeNameEdit, &QLineEdit::textChanged, this, &MainWindow::set_reg_block_codename);
    g->addWidget(codeNameEdit, REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second);

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    connect(customCNCheckBox, &QCheckBox::stateChanged, this, &MainWindow::set_regblk_codename_generation);
    g->addWidget(customCNCheckBox, REG_BLOCK_FIELD_COORD_GEN_CODENAME.first, REG_BLOCK_FIELD_COORD_GEN_CODENAME.second);

    QLabel* sizeLabel = new QLabel("Size (in addrs): ");
    g->addWidget(sizeLabel, REG_BLOCK_FIELD_COORD_SIZE.first, REG_BLOCK_FIELD_COORD_SIZE.second-1);

    QSpinBox* sizeEdit = new QSpinBox();
    connect(sizeEdit, &QSpinBox::valueChanged, this, &MainWindow::set_reg_block_size);
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
    regTable->setSelectionMode(QAbstractItemView::NoSelection);
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_OFFSET, new QTableWidgetItem("Offset"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_DESC, new QTableWidgetItem("Descwiption"));
    connect(regTable, &QTableWidget::itemDoubleClicked, this, &MainWindow::set_current_reg);
    g->addWidget(regTable, REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second, 1, 4);

    QPushButton* newregButton = new QPushButton("New Register");
    connect(newregButton, &QPushButton::clicked, this, &MainWindow::make_new_reg);
    g->addWidget(newregButton, REG_BLOCK_FIELD_COORD_NEWREGBTN.first, REG_BLOCK_FIELD_COORD_NEWREGBTN.second);

    QFrame* regFrame = new QFrame();
    regFrame->setLineWidth(5);
    QGridLayout* reggrid = new QGridLayout();
    printf("reggrid: %p\n", (void*)reggrid);
    fflush(stdout);
    regFrame->setLayout(reggrid);

    {
        QLabel* regNameLabel = new QLabel("Name: ");
        regNameLabel->setEnabled(false);
        reggrid->addWidget(regNameLabel, REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second-1);

        QLineEdit* nameEdit = new QLineEdit();
        connect(nameEdit, &QLineEdit::textEdited, this, &MainWindow::set_reg_name);
        nameEdit->setEnabled(false); //will set editable when register is tracked with this UI
        reggrid->addWidget(nameEdit, REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second);

        QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
        codeNameLabel->setEnabled(false);
        reggrid->addWidget(codeNameLabel, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second-1);

        QLineEdit* codeNameEdit = new QLineEdit();
        codeNameEdit->setEnabled(false);
        codeNameEdit->setReadOnly(true);
        codeNameEdit->setValidator(this->codename_validator);
        connect(codeNameEdit, &QLineEdit::textChanged, this, &MainWindow::set_reg_codename);
        reggrid->addWidget(codeNameEdit, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second);

        QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
        connect(customCNCheckBox, &QCheckBox::stateChanged, this, &MainWindow::set_reg_codename_gen);
        customCNCheckBox->setEnabled(false); //will set editable when register is tracked with this UI
        reggrid->addWidget(customCNCheckBox, REG_FIELD_COORD_GEN_CODENAME.first, REG_FIELD_COORD_GEN_CODENAME.second);

        QLabel* offsetLabel = new QLabel("Offset (in addrs): ");
        offsetLabel->setEnabled(false);
        reggrid->addWidget(offsetLabel, REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second-1);

        QSpinBox* offsetEdit = new QSpinBox();
        connect(offsetEdit, &QSpinBox::valueChanged, this, &MainWindow::set_reg_offset);
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

