#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <limits>

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>


#define REG_BLOCK_FIELD_COORD_NAME          (std::pair<int, int>(0, 1))
#define REG_BLOCK_FIELD_COORD_CODENAME      (std::pair<int, int>(0, 3))
#define REG_BLOCK_FIELD_COORD_GEN_CODENAME  (std::pair<int, int>(1, 3))
#define REG_BLOCK_FIELD_COORD_SIZE          (std::pair<int, int>(2, 1))
#define REG_BLOCK_FIELD_COORD_CN_COLL_WARN  (std::pair<int, int>(2, 3))
#define REG_BLOCK_FIELD_COORD_DESC_LABEL    (std::pair<int, int>(3, 0))
#define REG_BLOCK_FIELD_COORD_DESC          (std::pair<int, int>(4, 0))
#define REG_BLOCK_FIELD_COORD_REGTABLE      (std::pair<int, int>(5, 0))
#define REG_BLOCK_FIELD_COORD_NEWREGBTN     (std::pair<int, int>(6, 0))
#define REG_BLOCK_FIELD_COORD_SORTREGBTN    (std::pair<int, int>(6, 1))
#define REG_BLOCK_FIELD_COORD_REGFRAME      (std::pair<int, int>(7, 0))
#define REG_BLOCK_FIELD_COORD_SPACER        (std::pair<int, int>(8, 0))

#define REG_BLOCK_FIELD_WIDTH   (4)
#define REG_BLOCK_FIELD_HEIGHT  (9)

#define REG_FIELD_COORD_NAME            (std::pair<int, int>(0, 1))
#define REG_FIELD_COORD_CODENAME        (std::pair<int, int>(0, 3))
#define REG_FIELD_COORD_GEN_CODENAME    (std::pair<int, int>(1, 3))
#define REG_FIELD_COORD_OFFSET          (std::pair<int, int>(2, 1))
#define REG_FIELD_COORD_CN_COLL_WARN    (std::pair<int, int>(2, 3))
#define REG_FIELD_COORD_DESC_LABEL      (std::pair<int, int>(3, 0))
#define REG_FIELD_COORD_DESC            (std::pair<int, int>(4, 0))

#define REG_FIELD_WIDTH   (4)
#define REG_FIELD_HEIGHT  (5)

#define REG_TABLE_COL_NAME      (0)
#define REG_TABLE_COL_OFFSET    (1)
#define REG_TABLE_COL_DESC      (2)
#define REG_TABLE_COL_MAX       REG_TABLE_COL_DESC

MainWindow::MainWindow(void (*makeNewWindow)(QString), QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->makeNewWindow = makeNewWindow;

    connect(this->ui->actionSave_As, &QAction::triggered, this, &MainWindow::save);
    connect(this->ui->actionLoad, &QAction::triggered, this, &MainWindow::load);
    connect(this->ui->actionNew, &QAction::triggered, this, [=](){
        this->makeNewWindow("");
    });

    this->setWindowTitle("New Register Map");
}

MainWindow::~MainWindow()
{
    delete ui;
}

test_result_enum MainWindow::checkRBOffsetCollisions()
{
    test_result_enum final_result = OK;
    for (RegisterBlockController* rbc : this->reg_block_ctrls){
        test_result_enum result = this->checkOffsetCollisions(rbc);
        if (result != OK) final_result = ERROR;
    }
    return final_result;
}

test_result_enum MainWindow::checkOffsetCollisions(RegisterBlockController* rbc)
{
    bool reg_collision = false;
    QVector<int> colliding_reg_idxs;

    int num_regs = rbc->getNumRegs();

    for (int i = 0; i < num_regs; ++i){
        for (int j = 0; j < num_regs; ++j){
            if (i == j) continue;
            if (rbc->getRegOffset(i) == rbc->getRegOffset(j)){
                reg_collision = true;
                if (!colliding_reg_idxs.contains(i)) colliding_reg_idxs.push_back(i);
                if (!colliding_reg_idxs.contains(j)) colliding_reg_idxs.push_back(j);
            }
        }
    }
    if (reg_collision){
        QString warn_msg = "One or more Registers in " + rbc->getName() + " have the same offset.";
        for (int i : colliding_reg_idxs){
            warn_msg += "\nName: ";
            warn_msg += rbc->getRegName(i);
            warn_msg += "\tOffset: ";
            warn_msg += rbc->getRegOffsetAsString(i);
        }
        QMessageBox::warning(this, "Validation Failed: " + rbc->getName(), warn_msg);
    }
    return reg_collision ? ERROR : OK;
}

test_result_enum MainWindow::checkRBCodeNameCollisions()
{
    test_result_enum result = OK;

    QVector<RegisterBlockController*> colliding_rbs;
    QVector<RegisterBlockController*> empty_rbs;
    for (RegisterBlockController* p : this->reg_block_ctrls){
        for (RegisterBlockController* q : this->reg_block_ctrls){
            if (p == q) continue;
            if (!p->getCodeName().compare(q->getCodeName())){
                if(!colliding_rbs.contains(p)) colliding_rbs.push_back(p);
                if(!colliding_rbs.contains(q)) colliding_rbs.push_back(q);
            }
        }
        if (p->getCodeName().length() == 0){
            if(!empty_rbs.contains(p)) empty_rbs.push_back(p);
        }
    }
    if (colliding_rbs.length() > 0){
        QString warn_msg = "One or more Register Blocks have the same Source-Friendly name.";
        for (RegisterBlockController* c : colliding_rbs){
            warn_msg += "\nName: ";
            warn_msg += c->getName();
            warn_msg += "\tSource-Friendly Name: ";
            warn_msg += c->getCodeName();
        }
        QMessageBox::warning(this, "Validation Failed", warn_msg);
        result = ERROR;
    }
    if (empty_rbs.length() > 0){
        QString warn_msg = "One or more Register Blocks have no Source-Friendly Name.";
        for (RegisterBlockController* c : empty_rbs){
            warn_msg += "\nName: ";
            warn_msg += c->getName();
        }
        QMessageBox::warning(this, "Validation Failed", warn_msg);
        result = ERROR;
    }
    return result;
}

test_result_enum MainWindow::checkRBRegCodeNameCollisions()
{
    test_result_enum final_result = OK;
    for (RegisterBlockController* rbc : this->reg_block_ctrls){
        test_result_enum result = this->checkRegCodeNameCollisions(rbc);
        if (result != OK) final_result = ERROR;
    }
    return final_result;
}

test_result_enum MainWindow::checkRegCodeNameCollisions(RegisterBlockController* rbc)
{
    test_result_enum result = OK;

    QVector<int> colliding_reg_idxs;
    QVector<int> empty_reg_idxs;
    int num_regs = rbc->getNumRegs();

    for (int i = 0; i < num_regs; ++i){
        for (int j = 0; j < num_regs; ++j){
            if (i == j) continue;
            if (!(rbc->getRegCodeName(i).compare(rbc->getRegCodeName(j)))){
                if (!colliding_reg_idxs.contains(i)) colliding_reg_idxs.push_back(i);
                if (!colliding_reg_idxs.contains(j)) colliding_reg_idxs.push_back(j);
            }
        }
        if (rbc->getRegCodeName(i).length() == 0){
            if (!empty_reg_idxs.contains(i)) empty_reg_idxs.push_back(i);
        }
    }

    if (colliding_reg_idxs.length() > 0){
        QString warn_msg = "One or more Registers in " + rbc->getName() + " have the same Source-Friendly name.";
        for (int i : colliding_reg_idxs){
            warn_msg += "\nName: ";
            warn_msg += rbc->getRegName(i);
            warn_msg += "\tSource-Friendly Name: ";
            warn_msg += rbc->getRegCodeName(i);
        }
        QMessageBox::warning(this, "Validation Failed: " + rbc->getName(), warn_msg);
        result = ERROR;
    }

    if (empty_reg_idxs.length() > 0){
        QString warn_msg = "One or more Registers in " + rbc->getName() + " have no Source-Friendly name.";
        for (int i : empty_reg_idxs){
            warn_msg += "\nName: ";
            warn_msg += rbc->getRegName(i);
        }
        QMessageBox::warning(this, "Validation Failed: " + rbc->getName(), warn_msg);
        result = ERROR;
    }
    return result;
}

void MainWindow::print_toml_table(toml_value_t table, int tab_level = 0)
{
    if (!table.is_table()) return;
    for (std::pair<const std::string, toml_value_t>& kv : table.as_table()){
        for (int i = 0; i < tab_level; ++i){
            printf("\t");
        }
        printf("%s\n", kv.first.c_str());

        this->print_toml_table(kv.second, tab_level+1);
    }
}

void MainWindow::save()
{
    bool valid = true;

    //verify that no register block codenames collide or are empty
    valid &= this->checkRBCodeNameCollisions() == OK;

    //verify that no register codenames collide within register blocks (or are empty)
    valid &= this->checkRBRegCodeNameCollisions() == OK;

    //verify that no register offsets collide within register blocks
    valid &= this->checkRBOffsetCollisions() == OK;

    if (!valid) return;


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

    toml_value_t base_table{
        {"version_major", 0},
        {"version_minor", 1}
    };

    std::string toml_id;

    for (RegisterBlockController* p : this->reg_block_ctrls){

        toml_value_t reg_array;

        //we already verified that no offset collisions occur, so this is OK
        p->sortRegsByOffset();

        for (int i = 0; i < p->getNumRegs(); ++i){
            printf("Collecting register %s (0x%x)\n", p->getRegName(i).toUtf8().constData(), p->getRegOffset(i));
            toml_value_t reg_record{
                {"name", p->getRegName(i).toStdString()},
                {"codename", p->getRegCodeName(i).toStdString()},
                {"autogen_codename", p->getRegCodeNameGeneration(i) ? "true" : "false"},
                {"offset", p->getRegOffset(i)},
            };
            toml_id = std::to_string(p->getRegOffset(i)) + "_" + p->getRegCodeName(i).toStdString();
            reg_array[toml_id] = reg_record;
        }

        toml_value_t rb_table{
            {"name", p->getName().toStdString()},
            {"codename", p->getCodeName().toStdString()},
            {"autogen_codename", p->getCodeNameGeneration() ? "true" : "false"},
            {"size", p->getSize()},
            {"registers", reg_array}
        };
        toml_id = reg_block_prefix + p->getCodeName().toStdString();
        base_table[toml_id] = rb_table;
    }

    std_stream << base_table << std::endl;

    savefilestream << std_stream.str().c_str();

    this->setWindowTitle(QFileInfo(save_file.fileName()).fileName());

    save_file.close();
}

void MainWindow::load()
{

    QMessageBox msgBox;
    msgBox.setText("Load in new window?");
    msgBox.setInformativeText("Selecting 'No' will discard all changes in the current window.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int selected_opt = msgBox.exec();

    QString start_path = QStandardPaths::displayName(QStandardPaths::DesktopLocation);
    QString load_filename = QFileDialog::getOpenFileName(this, "Load TOML File", start_path, "TOML Files (*.toml)");
    printf("Load from: %s\n", load_filename.toUtf8().constData());

    if (selected_opt == QMessageBox::Yes){
        //make a new window and load it with the selected TOML file
        this->makeNewWindow(load_filename);
    } else {
        //wipe this window and load the selected TOML file
        this->load_file(load_filename);
    }

}

void MainWindow::load_file(QString load_filename)
{

    if (load_filename.length() == 0){
        printf("Empty load filename, skipping load...\n");
        return;
    }

    QFile load_file(load_filename);
    if (!(load_file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text))){
        QMessageBox::warning(this, "File load Failed", "Failed to load file");
        return;
    }

    toml_value_t base_table;

    try {
        base_table = toml::parse(load_filename.toUtf8().constData());

        printf("File contents:\n");
        this->print_toml_table(base_table);

        int vmaj = toml::find<int>(base_table, "version_major");
        int vmin = toml::find<int>(base_table, "version_minor");

        if (vmaj == 0 && vmin == 1){
            printf("Current version!\n");
        } else {
            fprintf(stderr, "Version %d.%d is not yet supported (this binary is version %d.%d\n", vmaj, vmin, 0, 1);
            return;
        }

        //----------------------------------------------------------
        //This is the point of no return (pun intended)
        //----------------------------------------------------------

        //remove all tabs except main tab
        //(doesnt delete them yet but thats fine)
        while(this->ui->tabWidget->count() > 1){
            this->ui->tabWidget->removeTab(1);
        }

        for (std::pair<const std::string, toml_value_t>& kv : base_table.as_table()){
            const std::string key = kv.first;
            toml_value_t val = kv.second;

            if (!key.substr(0, reg_block_prefix.length()).compare(reg_block_prefix)){
                //this is a register block

                std::string name;
                try {
                    name = toml::find<std::string>(val, "name");
                } catch (std::out_of_range& e){
                    name = key;
                }

                std::string codename;
                try {
                    codename = toml::find<std::string>(val, "codename");
                } catch (std::out_of_range& e){
                    codename = key;
                }

                bool gen_codename;
                try {
                    gen_codename = toml::find<std::string>(val, "autogen_codename").compare("false");
                } catch (std::out_of_range& e){
                    gen_codename = true;
                }

                addr_t size;
                try {
                    size = toml::find<addr_t>(val, "size");
                } catch (std::out_of_range& e){
                    //TODO: try best to figure out size from register offsets?
                    //if this isnt in the TOML then this should probably just error out to be honest.
                    size = 0;
                }

                toml_value_t registers;
                try {
                    registers = toml::find(val, "registers");
                } catch (std::out_of_range& e){
                    //TODO: this should deffo throw an error message. probably not a popup every time though.
                }

                this->on_new_reg_block_btn_clicked();
                RegisterBlockController* rbc = this->reg_block_ctrls.at(this->reg_block_ctrls.size()-1);

                rbc->setName(name.c_str());
                rbc->setCodeNameGeneration(gen_codename);
                rbc->setCodeName(codename.c_str());
                rbc->setSize(size);

                for (std::pair<const std::string, toml_value_t>& kv : registers.as_table()){
                    const std::string key = kv.first;
                    toml_value_t val = kv.second;

                    std::string name;
                    try {
                        name = toml::find<std::string>(val, "name");
                    } catch (std::out_of_range& e){
                        name = key;
                    }

                    std::string codename;
                    try {
                        codename = toml::find<std::string>(val, "codename");
                    } catch (std::out_of_range& e){
                        codename = key;
                    }

                    bool gen_codename;
                    try {
                        gen_codename = toml::find<std::string>(val, "autogen_codename").compare("false");
                    } catch (std::out_of_range& e){
                        gen_codename = true;
                    }

                    addr_t offset;
                    try {
                        offset = toml::find<addr_t>(val, "offset");
                    } catch (std::out_of_range& e){
                        //TODO: this should be an error.
                        offset = 0;
                    }

                    rbc->makeNewReg();
                    int new_reg_idx = rbc->getNumRegs() - 1;
                    rbc->setCurrRegIdx(new_reg_idx);

                    rbc->setRegName(name.c_str());
                    rbc->setRegCodeNameGeneration(gen_codename);
                    rbc->setRegCodeName(codename.c_str());
                    rbc->setRegOffset(offset);
                }
            }
        }

    } catch (std::runtime_error& e){
        fprintf(stderr, "%s:%d: TOML Parse failed (runtime error): %s", __FILE__, __LINE__, e.what());
        QMessageBox::warning(this, "File load Failed", "Failed to parse file " + load_filename + "\n");
        return;
    } catch (std::out_of_range& e){
        fprintf(stderr, "%s:%d: TOML Parse failed (out of range error): %s", __FILE__, __LINE__, e.what());
        QMessageBox::warning(this, "File load Failed", "Failed to parse file " + load_filename + "\n");
        return;
    }

    this->setWindowTitle(QFileInfo(load_file.fileName()).fileName());

    load_file.close();
}

void MainWindow::changeMade()
{
    QString windowTitle = this->windowTitle();
    if (windowTitle.at(windowTitle.length()-1) != '*'){
        //this indicates an unsaved change exists
        this->setWindowTitle(windowTitle + "*");
    }
}


void MainWindow::on_new_reg_block_btn_clicked()
{
    QWidget* w = new QWidget();
    this->ui->tabWidget->addTab(w, "Register Block");

    RegisterBlockController* rbc = new RegisterBlockController(w);
    this->reg_block_ctrls.push_back(rbc);

    connect(rbc, &RegisterBlockController::nameChanged, this->ui->tabWidget, [=](const QString& new_name){
        this->ui->tabWidget->setTabText(this->ui->tabWidget->indexOf(w), new_name);
    });

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
    connect(codeNameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setCodeName);
    connect(rbc, &RegisterBlockController::codeNameChanged, codeNameEdit, &QLineEdit::setText);
    g->addWidget(codeNameEdit, REG_BLOCK_FIELD_COORD_CODENAME.first, REG_BLOCK_FIELD_COORD_CODENAME.second);

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    //WARNING: this relies on the fact that the enum describing the "checked" states of a checkbox place "unchecked" as the 0 value,
    //and other levels of being checked at nonzero values, which will map to the bool type as checked -> true, unchecked -> false.
    //if that enum changes, this breaks. I dont feel like making this super robust though cause i dont want to include Qt GUI stuff in my controller.
    //wah wah.
    connect(customCNCheckBox, &QCheckBox::stateChanged, rbc, [=](int state){
        if (state == Qt::CheckState::Checked){
            rbc->setCodeNameGeneration(false);
        } else {
            rbc->setCodeNameGeneration(true);
        }
    });
    connect(rbc, &RegisterBlockController::codeNameGenerationChanged, customCNCheckBox, [=](bool gen_code_name){
        customCNCheckBox->setChecked(!gen_code_name);
        codeNameEdit->setReadOnly(gen_code_name);
    });
    g->addWidget(customCNCheckBox, REG_BLOCK_FIELD_COORD_GEN_CODENAME.first, REG_BLOCK_FIELD_COORD_GEN_CODENAME.second);

    QLabel* CNCollisionWarningLabel = new QLabel("");
    CNCollisionWarningLabel->setStyleSheet("QLabel { color : red; }");
    g->addWidget(CNCollisionWarningLabel, REG_BLOCK_FIELD_COORD_CN_COLL_WARN.first, REG_BLOCK_FIELD_COORD_CN_COLL_WARN.second);

    QLabel* sizeLabel = new QLabel("Size (in addrs): ");
    g->addWidget(sizeLabel, REG_BLOCK_FIELD_COORD_SIZE.first, REG_BLOCK_FIELD_COORD_SIZE.second-1);

    QSpinBox* sizeEdit = new QSpinBox();
    connect(sizeEdit, &QSpinBox::valueChanged, rbc, &RegisterBlockController::setSize);
    connect(rbc, &RegisterBlockController::sizeChanged, sizeEdit, [=](addr_t new_size){
        if ((int)new_size != sizeEdit->value()){
            sizeEdit->setValue(new_size);
        }
    });
    sizeEdit->setMinimum(0);
    sizeEdit->setMaximum(std::numeric_limits<int>::max());
    g->addWidget(sizeEdit, REG_BLOCK_FIELD_COORD_SIZE.first, REG_BLOCK_FIELD_COORD_SIZE.second);

    sizeEdit->setValue(rbc->getSize());

    QLabel* descLabel = new QLabel("Decription: ");
    g->addWidget(descLabel, REG_BLOCK_FIELD_COORD_DESC_LABEL.first, REG_BLOCK_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    g->addWidget(descEdit, REG_BLOCK_FIELD_COORD_DESC.first, REG_BLOCK_FIELD_COORD_DESC.second, 1, REG_BLOCK_FIELD_WIDTH);

    QTableWidget* regTable = new QTableWidget(0, 3);
    regTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    regTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    regTable->setSelectionMode(QAbstractItemView::SingleSelection);
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_OFFSET, new QTableWidgetItem("Offset"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_DESC, new QTableWidgetItem("Descwiption"));
    connect(regTable, &QTableWidget::itemDoubleClicked, rbc, [=](QTableWidgetItem* item){
        int row = regTable->row(item);
        rbc->setCurrRegIdx(row);
    });

    connect(rbc, &RegisterBlockController::currRegIdxChanged, regTable, &QTableWidget::selectRow);

    //this forces the highlighted row to always be the one we're editing
    connect(regTable, &QTableWidget::itemSelectionChanged, this, [=](){
        int curr_row = rbc->getCurrRegIdx();
        for (QTableWidgetItem* twi : regTable->selectedItems()){
            if (regTable->row(twi) != curr_row){
                regTable->clearSelection();
                regTable->selectRow(curr_row);
                break;
            }
        }
    });

    g->addWidget(regTable, REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second, 1, REG_BLOCK_FIELD_WIDTH);

    QPushButton* newregButton = new QPushButton("New Register");
    connect(newregButton, &QPushButton::clicked, rbc, &RegisterBlockController::makeNewReg);
    g->addWidget(newregButton, REG_BLOCK_FIELD_COORD_NEWREGBTN.first, REG_BLOCK_FIELD_COORD_NEWREGBTN.second);

    QPushButton* sortRegsButton = new QPushButton("Sort Registers by Offset");
    connect(sortRegsButton, &QPushButton::clicked, rbc, [=](){
        if (this->checkOffsetCollisions(rbc) != OK) return;
        rbc->sortRegsByOffset();
    });
    g->addWidget(sortRegsButton, REG_BLOCK_FIELD_COORD_SORTREGBTN.first, REG_BLOCK_FIELD_COORD_SORTREGBTN.second);

    QFrame* regFrame = new QFrame();
    regFrame->setLineWidth(5);

    //connect up new register functionality here so we can reference regFrame
    connect(rbc, &RegisterBlockController::regCreated, regTable, [=]
            (const QString& name, addr_t offset, const QString& description){

        Q_UNUSED(offset);

        regTable->setRowCount(regTable->rowCount()+1);
        int curr_table_row = regTable->rowCount() - 1;

        QTableWidgetItem* name_item = new QTableWidgetItem(name);
        QTableWidgetItem* offset_item = new QTableWidgetItem(rbc->getRegOffsetAsString(rbc->getNumRegs()-1));
        QTableWidgetItem* desc_item = new QTableWidgetItem(description);

        connect(rbc, &RegisterBlockController::regNameChanged, regTable, [=](const QString& new_name){
            if (rbc->getCurrRegIdx() == curr_table_row){
                name_item->setText(new_name);
            }
        });
        connect(rbc, &RegisterBlockController::regOffsetChanged, regTable, [=](addr_t new_offset){
            Q_UNUSED(new_offset);
            if (rbc->getCurrRegIdx() == curr_table_row){
                offset_item->setText(rbc->getCurrRegOffsetAsString());
            }
        });
        //TODO: add connection for register description change

        regTable->setItem(curr_table_row, REG_TABLE_COL_NAME, name_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_OFFSET, offset_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_DESC, desc_item);

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
        connect(codeNameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setRegCodeName);
        connect(rbc, &RegisterBlockController::regCodeNameChanged, codeNameEdit, &QLineEdit::setText);
        reggrid->addWidget(codeNameEdit, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second);

        QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
        connect(customCNCheckBox, &QCheckBox::stateChanged, rbc, [=](int state){
            if (state == Qt::CheckState::Checked){
                rbc->setRegCodeNameGeneration(false);
            } else {
                rbc->setRegCodeNameGeneration(true);
            }
        });
        connect(rbc, &RegisterBlockController::regCodeNameGenerationChanged, customCNCheckBox, [=](bool gen_code_name){
            customCNCheckBox->setChecked(!gen_code_name);
            codeNameEdit->setReadOnly(gen_code_name);
        });
        customCNCheckBox->setEnabled(false); //will set editable when register is tracked with this UI
        reggrid->addWidget(customCNCheckBox, REG_FIELD_COORD_GEN_CODENAME.first, REG_FIELD_COORD_GEN_CODENAME.second);

//        QLabel* CNCollisionWarningLabel = new QLabel("");
//        CNCollisionWarningLabel->setStyleSheet("QLabel { color : red; }");
//        reggrid->addWidget(CNCollisionWarningLabel, REG_FIELD_COORD_CN_COLL_WARN.first, REG_FIELD_COORD_CN_COLL_WARN.second);

        QLabel* offsetLabel = new QLabel("Offset (in addrs): ");
        offsetLabel->setEnabled(false);
        reggrid->addWidget(offsetLabel, REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second-1);

        QSpinBox* offsetEdit = new QSpinBox();
        connect(offsetEdit, &QSpinBox::valueChanged, rbc, &RegisterBlockController::setRegOffset);
        connect(rbc, &RegisterBlockController::regOffsetChanged, offsetEdit, [=](addr_t new_offset){
            if (new_offset != (addr_t)offsetEdit->value()){
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

    g->addWidget(regFrame, REG_BLOCK_FIELD_COORD_REGFRAME.first, REG_BLOCK_FIELD_COORD_REGFRAME.second, 1, REG_BLOCK_FIELD_WIDTH);

    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    g->addItem(spacer, REG_BLOCK_FIELD_COORD_SPACER.first, REG_BLOCK_FIELD_COORD_SPACER.second);

    //notify of any actual data changes
    connect(rbc, &RegisterBlockController::changeMade, this, &MainWindow::changeMade);

    rbc->setName("Register Block " + QString::number(this->reg_block_ctrls.size()-1));

}

