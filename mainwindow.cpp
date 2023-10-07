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
#include <QStackedWidget>


#define REG_BLOCK_FIELD_COORD_NAME          (std::pair<int, int>(0, 1))
#define REG_BLOCK_FIELD_COORD_CODENAME      (std::pair<int, int>(0, 3))
#define REG_BLOCK_FIELD_COORD_GEN_CODENAME  (std::pair<int, int>(1, 3))
#define REG_BLOCK_FIELD_COORD_SIZE          (std::pair<int, int>(2, 1))
//#define REG_BLOCK_FIELD_COORD_CN_COLL_WARN  (std::pair<int, int>(2, 3))
#define REG_BLOCK_FIELD_COORD_DESC_LABEL    (std::pair<int, int>(3, 0))
#define REG_BLOCK_FIELD_COORD_DESC          (std::pair<int, int>(4, 0))
#define REG_BLOCK_FIELD_COORD_REGFRAME      (std::pair<int, int>(5, 0))
#define REG_BLOCK_FIELD_COORD_SPACER        (std::pair<int, int>(6, 0))

#define REG_BLOCK_FIELD_WIDTH   (4)
#define REG_BLOCK_FIELD_HEIGHT  (7)

#define REG_FIELD_COORD_REGTABLE            (std::pair<int, int>(0, 0))
#define REG_FIELD_COORD_NEWREGBTN           (std::pair<int, int>(1, 0))
#define REG_FIELD_COORD_SORTREGBTN          (std::pair<int, int>(1, 1))
#define REG_FIELD_COORD_NAME                (std::pair<int, int>(2, 1))
#define REG_FIELD_COORD_CODENAME            (std::pair<int, int>(2, 3))
#define REG_FIELD_COORD_GEN_CODENAME        (std::pair<int, int>(3, 3))
#define REG_FIELD_COORD_OFFSET              (std::pair<int, int>(4, 1))
#define REG_FIELD_COORD_BITLEN              (std::pair<int, int>(4, 3))
#define REG_FIELD_COORD_BYTELEN             (std::pair<int, int>(5, 3))
#define REG_FIELD_COORD_DESC_LABEL          (std::pair<int, int>(6, 0))
#define REG_FIELD_COORD_DESC                (std::pair<int, int>(7, 0))
#define REG_FIELD_COORD_BITFIELD_FRAME      (std::pair<int, int>(8, 0))

#define REG_FIELD_WIDTH   (4)
#define REG_FIELD_HEIGHT  (9)

#define REG_TABLE_COL_NAME      (0)
#define REG_TABLE_COL_OFFSET    (1)
#define REG_TABLE_COL_DESC      (2)
#define REG_TABLE_COL_MAX       REG_TABLE_COL_DESC

#define BITFIELD_FIELD_COORD_BITFIELD_TABLE     (std::pair<int, int>(0, 0))
#define BITFIELD_FIELD_COORD_NEW_BITFIELD_BTN   (std::pair<int, int>(1, 0))
#define BITFIELD_FIELD_COORD_NAME               (std::pair<int, int>(2, 1))
#define BITFIELD_FIELD_COORD_CODENAME           (std::pair<int, int>(2, 3))
#define BITFIELD_FIELD_COORD_GEN_CODENAME       (std::pair<int, int>(3, 3))
#define BITFIELD_FIELD_COORD_RANGE_HIGH         (std::pair<int, int>(4, 1))
#define BITFIELD_FIELD_COORD_RANGE_LOW          (std::pair<int, int>(4, 3))
#define BITFIELD_FIELD_COORD_DESC_LABEL         (std::pair<int, int>(5, 0))
#define BITFIELD_FIELD_COORD_DESC               (std::pair<int, int>(6, 0))

#define BITFIELD_FIELD_WIDTH   (4)
#define BITFIELD_FIELD_HEIGHT  (7)

#define BITFIELD_TABLE_COL_NAME     (0)
#define BITFIELD_TABLE_COL_RANGE    (1)
#define BITFIELD_TABLE_COL_DESC     (2)
#define BITFIELD_TABLE_COL_MAX      BITFIELD_TABLE_COL_DESC

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

    //TODO: store version of app and write that out instead
    toml_value_t base_table{
        {vmaj_key, 0},
        {vmin_key, 1}
    };

    std::string toml_id;

    for (RegisterBlockController* p : this->reg_block_ctrls){

        toml_value_t reg_array;

        //we already verified that no offset collisions occur, so this is OK
        p->sortRegsByOffset();

        for (int i = 0; i < p->getNumRegs(); ++i){

            toml_value_t bitfield_array;

            for (int b = 0; b < p->getNumBitFields(i); ++b){

                BitField& bf = p->getRegBitField(i, b);

                toml_value_t bitfield_record{
                    {RegisterBlockController::bitfield_name_key, bf.name},
                    {RegisterBlockController::bitfield_codename_key, bf.codename},
                    {RegisterBlockController::bitfield_codenamegen_key,
                        p->getRegBitFieldCodeNameGeneration(i, b) ? "true" : "false"},
                    {RegisterBlockController::bitfield_desc_key, bf.description},
                    {RegisterBlockController::bitfield_high_idx_key, bf.high_index},
                    {RegisterBlockController::bitfield_low_idx_key, bf.low_index}
                };

                toml_id = std::to_string(bf.low_index) + "_" + bf.name;
                bitfield_array[toml_id] = bitfield_record;
            }

//            printf("Collecting register %s (0x%x)\n", p->getRegName(i).toUtf8().constData(), p->getRegOffset(i));
            toml_value_t reg_record{
                {RegisterBlockController::reg_name_key, p->getRegName(i).toStdString()},
                {RegisterBlockController::reg_codename_key, p->getRegCodeName(i).toStdString()},
                {RegisterBlockController::reg_codenamegen_key, p->getRegCodeNameGeneration(i) ? "true" : "false"},
                {RegisterBlockController::reg_offset_key, p->getRegOffset(i)},
                {RegisterBlockController::reg_bitlen_key, p->getRegBitLen(i)},
                {RegisterBlockController::reg_desc_key, p->getRegDescription(i).toStdString()}
            };

            //only add bit field array if any exist to add, adding empty arrays is bad in TOML
            if (p->getNumBitFields(i) > 0){
                reg_record.as_table()[RegisterBlockController::reg_bitfields_key] = bitfield_array;
            }

            toml_id = std::to_string(p->getRegOffset(i)) + "_" + p->getRegCodeName(i).toStdString();
            reg_array[toml_id] = reg_record;
        }

        toml_value_t rb_table{
            {RegisterBlockController::name_key, p->getName().toStdString()},
            {RegisterBlockController::codename_key, p->getCodeName().toStdString()},
            {RegisterBlockController::codenamegen_key, p->getCodeNameGeneration() ? "true" : "false"},
            {RegisterBlockController::size_key, p->getSize()},
            {RegisterBlockController::desc_key, p->getDescription().toStdString()}
        };

        //only add register array if any exist to add, adding empty arrays is bad in TOML
        if (p->getNumRegs() > 0){
            rb_table.as_table()[RegisterBlockController::reg_key] = reg_array;
        }

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

        //TODO: make method of storing old load methods and call upon those to translate to the current object structure
        int vmaj = toml::find<int>(base_table, vmaj_key);
        int vmin = toml::find<int>(base_table, vmin_key);

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
        for (RegisterBlockController* rbc : this->reg_block_ctrls){
            rbc->deleteLater();
        }
        this->reg_block_ctrls.clear();

        for (std::pair<const std::string, toml_value_t>& kv : base_table.as_table()){
            const std::string key = kv.first;
            toml_value_t val = kv.second;

            if (!key.substr(0, reg_block_prefix.length()).compare(reg_block_prefix)){
                //this is a register block
                this->loadRegisterBlock(val, key);
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

void MainWindow::loadRegisterBlock(toml_value_t reg_block_table, std::string table_key)
{
    std::string name;
    try {
        name = toml::find<std::string>(reg_block_table, RegisterBlockController::name_key);
    } catch (std::out_of_range& e){
        name = table_key;
    }

    std::string codename;
    try {
        codename = toml::find<std::string>(reg_block_table, RegisterBlockController::codename_key);
    } catch (std::out_of_range& e){
        codename = table_key;
    }

    bool gen_codename;
    try {
        gen_codename = toml::find<std::string>(reg_block_table, RegisterBlockController::codenamegen_key).compare("false");
    } catch (std::out_of_range& e){
        gen_codename = true;
    }

    std::string description;
    try {
        description = toml::find<std::string>(reg_block_table, RegisterBlockController::desc_key);
    } catch (std::out_of_range& e){
        description = "";
    }

    addr_t size;
    try {
        size = toml::find<addr_t>(reg_block_table, RegisterBlockController::size_key);
    } catch (std::out_of_range& e){
        //TODO: try best to figure out size from register offsets?
        //if this isnt in the TOML then this should probably just error out to be honest.
        size = 0;
    }

    toml_value_t registers;
    try {
        registers = toml::find(reg_block_table, RegisterBlockController::reg_key);
    } catch (std::out_of_range& e){
        //TODO: this should deffo throw an error message. probably not a popup every time though.
    }

    this->on_new_reg_block_btn_clicked();
    RegisterBlockController* rbc = this->reg_block_ctrls.at(this->reg_block_ctrls.size()-1);

    rbc->setName(name.c_str());
    rbc->setCodeNameGeneration(gen_codename);
    rbc->setCodeName(codename.c_str());
    rbc->setDescription(description.c_str());
    rbc->setSize(size);

    if (!registers.is_table()) return;

    for (std::pair<const std::string, toml_value_t>& kv : registers.as_table()){
        const std::string key = kv.first;
        toml_value_t val = kv.second;

        this->loadRegister(val, key, rbc);
    }
}

void MainWindow::loadRegister(toml_value_t reg_table, std::string table_key, RegisterBlockController* rbc)
{
    std::string name;
    try {
        name = toml::find<std::string>(reg_table, RegisterBlockController::reg_name_key);
    } catch (std::out_of_range& e){
        name = table_key;
    }

    std::string codename;
    try {
        codename = toml::find<std::string>(reg_table, RegisterBlockController::reg_codename_key);
    } catch (std::out_of_range& e){
        codename = table_key;
    }

    bool gen_codename;
    try {
        gen_codename = toml::find<std::string>(reg_table, RegisterBlockController::reg_codenamegen_key).compare("false");
    } catch (std::out_of_range& e){
        gen_codename = true;
    }

    addr_t offset;
    try {
        offset = toml::find<addr_t>(reg_table, RegisterBlockController::reg_offset_key);
    } catch (std::out_of_range& e){
        //TODO: this should be an error.
        offset = 0;
    }

    uint32_t bit_len;
    try {
        bit_len = toml::find<uint32_t>(reg_table, RegisterBlockController::reg_bitlen_key);
    } catch (std::out_of_range& e){
        //TODO: this should be an error.
        bit_len = 8;
    }

    std::string description;
    try {
        description = toml::find<std::string>(reg_table, RegisterBlockController::reg_desc_key);
    } catch (std::out_of_range& e){
        description = "";
    }

    rbc->makeNewReg();
    int new_reg_idx = rbc->getNumRegs() - 1;
    rbc->setCurrRegIdx(new_reg_idx);

    rbc->setRegName(name.c_str());
    rbc->setRegCodeNameGeneration(gen_codename);
    rbc->setRegCodeName(codename.c_str());
    rbc->setRegOffset(offset);
    rbc->setRegBitLen(bit_len);
    rbc->setRegDescription(description.c_str());

    toml_value_t bitfield_table;
    try {
        bitfield_table = toml::find(reg_table, RegisterBlockController::reg_bitfields_key);
    } catch (std::out_of_range& e){
        //no bitfields? no problem! Just dont make any
    }

    if (!bitfield_table.is_table()) return;

    for (std::pair<const std::string, toml_value_t>& kv : bitfield_table.as_table()){
        const std::string key = kv.first;
        toml_value_t val = kv.second;

        this->loadBitField(val, key, rbc);
    }
}

void MainWindow::loadBitField(toml_value_t bitfield_table, std::string table_key, RegisterBlockController* rbc)
{
    std::string name;
    try {
        name = toml::find<std::string>(bitfield_table, RegisterBlockController::bitfield_name_key);
    } catch (std::out_of_range& e){
        name = table_key;
    }

    std::string codename;
    try {
        codename = toml::find<std::string>(bitfield_table, RegisterBlockController::bitfield_codename_key);
    } catch (std::out_of_range& e){
        codename = table_key;
    }

    bool gen_codename;
    try {
        gen_codename = toml::find<std::string>(bitfield_table, RegisterBlockController::bitfield_codenamegen_key).compare("false");
    } catch (std::out_of_range& e){
        gen_codename = true;
    }

    std::string description;
    try {
        description = toml::find<std::string>(bitfield_table, RegisterBlockController::bitfield_desc_key);
    } catch (std::out_of_range& e){
        description = "";
    }

    uint32_t high_idx;
    try {
        high_idx = toml::find<uint32_t>(bitfield_table, RegisterBlockController::bitfield_high_idx_key);
    } catch (std::out_of_range& e){
        high_idx = 0;
    }

    uint32_t low_idx;
    try {
        low_idx = toml::find<uint32_t>(bitfield_table, RegisterBlockController::bitfield_low_idx_key);
    } catch (std::out_of_range& e){
        low_idx = 0;
    }

    if (low_idx > high_idx){
        uint32_t new_low = high_idx;
        printf(
            "Adjusting invalid bit range: %s -> %s\n",
            RegisterBlockController::getBitRangeAsString(low_idx, high_idx).toUtf8().constData(),
            RegisterBlockController::getBitRangeAsString(new_low, high_idx).toUtf8().constData()
            );
        low_idx = new_low;
    }

    rbc->makeNewBitField();
    int new_bitfield_idx = rbc->getCurrNumBitFields() - 1;
    rbc->setCurrBitFieldIdx(new_bitfield_idx);

    rbc->setBitFieldName(name.c_str());
    rbc->setBitFieldCodeNameGeneration(gen_codename);
    rbc->setBitFieldCodeName(codename.c_str());
    rbc->setBitFieldDescription(description.c_str());
    rbc->setBitFieldRange(low_idx, high_idx);
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
    QWidget* w = this->makeNewRegBlockTab();
    this->ui->tabWidget->addTab(w, "");
    this->reg_block_ctrls.at(this->reg_block_ctrls.size()-1)->setName(
        "Register Block " + QString::number(this->reg_block_ctrls.size()-1)
    );
}

QWidget* MainWindow::makeNewRegBlockTab(){

    QWidget* w = new QWidget();

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

    //    QLabel* CNCollisionWarningLabel = new QLabel("");
    //    CNCollisionWarningLabel->setStyleSheet("QLabel { color : red; }");
    //    g->addWidget(CNCollisionWarningLabel, REG_BLOCK_FIELD_COORD_CN_COLL_WARN.first, REG_BLOCK_FIELD_COORD_CN_COLL_WARN.second);

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

    QLabel* descLabel = new QLabel("Description: ");
    g->addWidget(descLabel, REG_BLOCK_FIELD_COORD_DESC_LABEL.first, REG_BLOCK_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    connect(descEdit, &QTextEdit::textChanged, rbc, [=](){
        rbc->setDescription(descEdit->toPlainText());
    });
    connect(rbc, &RegisterBlockController::descriptionChanged, descEdit, [=](const QString& new_desc){
        if (descEdit->toPlainText().compare(new_desc)){
            descEdit->setText(new_desc);
        }
    });
    g->addWidget(descEdit, REG_BLOCK_FIELD_COORD_DESC.first, REG_BLOCK_FIELD_COORD_DESC.second, 1, REG_BLOCK_FIELD_WIDTH);

    QFrame* regFrame = this->makeNewRegFrame(rbc);

    g->addWidget(regFrame, REG_BLOCK_FIELD_COORD_REGFRAME.first, REG_BLOCK_FIELD_COORD_REGFRAME.second, 1, REG_BLOCK_FIELD_WIDTH);

//    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
//    g->addItem(spacer, REG_BLOCK_FIELD_COORD_SPACER.first, REG_BLOCK_FIELD_COORD_SPACER.second);

    //notify of any actual data changes
    connect(rbc, &RegisterBlockController::changeMade, this, &MainWindow::changeMade);

    return w;
}

QFrame* MainWindow::makeNewRegFrame(RegisterBlockController* rbc){

    QFrame* regFrame = new QFrame();
    regFrame->setLineWidth(5);

    QGridLayout* reggrid = new QGridLayout();
    regFrame->setLayout(reggrid);

    QTableWidget* regTable = new QTableWidget(0, 3);
    regTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    regTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    regTable->setSelectionMode(QAbstractItemView::SingleSelection);
    regTable->setAutoScroll(false);
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_OFFSET, new QTableWidgetItem("Offset"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_DESC, new QTableWidgetItem("Description"));
    connect(regTable, &QTableWidget::itemClicked, rbc, [=](QTableWidgetItem* item){
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

    reggrid->addWidget(regTable, REG_FIELD_COORD_REGTABLE.first, REG_FIELD_COORD_REGTABLE.second, 1, REG_BLOCK_FIELD_WIDTH);

    QPushButton* newregButton = new QPushButton("New Register");
    connect(newregButton, &QPushButton::clicked, rbc, &RegisterBlockController::makeNewReg);
    reggrid->addWidget(newregButton, REG_FIELD_COORD_NEWREGBTN.first, REG_FIELD_COORD_NEWREGBTN.second);

    QPushButton* sortRegsButton = new QPushButton("Sort Registers by Offset");
    connect(sortRegsButton, &QPushButton::clicked, rbc, [=](){
        if (this->checkOffsetCollisions(rbc) != OK) return;
        rbc->sortRegsByOffset();
    });
    reggrid->addWidget(sortRegsButton, REG_FIELD_COORD_SORTREGBTN.first, REG_FIELD_COORD_SORTREGBTN.second);

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

    QLabel* bitLenLabel = new QLabel("Size (bits): ");
    bitLenLabel->setEnabled(false);
    reggrid->addWidget(bitLenLabel, REG_FIELD_COORD_BITLEN.first, REG_FIELD_COORD_BITLEN.second-1);

    QSpinBox* bitLenEdit = new QSpinBox();
    bitLenEdit->setValue(1);
    bitLenEdit->setMinimum(1);
    bitLenEdit->setMaximum(std::numeric_limits<int>::max());
    bitLenEdit->setEnabled(false); //will set editable when register is tracked with this UI
    connect(bitLenEdit, &QSpinBox::valueChanged, rbc, [=](int new_val){
        if (bitLenEdit->hasFocus()){
            rbc->setRegBitLen(new_val);
        }
    });
    connect(rbc, &RegisterBlockController::regBitLenChanged, bitLenEdit, [=](uint32_t new_bitlen){
        if (new_bitlen != (uint32_t)bitLenEdit->value()){
            bitLenEdit->setValue(new_bitlen);
        }
    });
    reggrid->addWidget(bitLenEdit, REG_FIELD_COORD_BITLEN.first, REG_FIELD_COORD_BITLEN.second);

    QLabel* byteLenLabel = new QLabel("Size (bytes): ");
    byteLenLabel->setEnabled(false);
    reggrid->addWidget(byteLenLabel, REG_FIELD_COORD_BYTELEN.first, REG_FIELD_COORD_BYTELEN.second-1);

    QSpinBox* byteLenEdit = new QSpinBox();
    byteLenEdit->setValue(1);
    byteLenEdit->setMinimum(1);
    byteLenEdit->setMaximum(std::numeric_limits<int>::max());
    byteLenEdit->setEnabled(false); //will set editable when register is tracked with this UI
    connect(byteLenEdit, &QSpinBox::valueChanged, rbc, [=](int new_val){
        if (byteLenEdit->hasFocus()){
            rbc->setRegBitLen(new_val*BITS_PER_BYTE);
        }
    });
    connect(rbc, &RegisterBlockController::regBitLenChanged, byteLenEdit, [=](uint32_t new_bitlen){
        uint32_t new_bytelen = (uint32_t)ceil((float)new_bitlen*1.0/BITS_PER_BYTE);
        if (new_bytelen != (uint32_t)byteLenEdit->value()){
            byteLenEdit->setValue(new_bytelen);
        }
    });
    reggrid->addWidget(byteLenEdit, REG_FIELD_COORD_BYTELEN.first, REG_FIELD_COORD_BYTELEN.second);

    QLabel* descLabel = new QLabel("Description: ");
    reggrid->addWidget(descLabel, REG_FIELD_COORD_DESC_LABEL.first, REG_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    connect(descEdit, &QTextEdit::textChanged, rbc, [=](){
        rbc->setRegDescription(descEdit->toPlainText());
    });
    connect(rbc, &RegisterBlockController::regDescriptionChanged, descEdit, [=](const QString& new_desc){
        if (descEdit->toPlainText().compare(new_desc)){
            descEdit->setText(new_desc);
        }
    });
    reggrid->addWidget(descEdit, REG_FIELD_COORD_DESC.first, REG_FIELD_COORD_DESC.second, 1, REG_FIELD_WIDTH);

    QStackedWidget* bitFieldStack = new QStackedWidget();
    connect(rbc, &RegisterBlockController::currRegIdxChanged, bitFieldStack, &QStackedWidget::setCurrentIndex);

    reggrid->addWidget(bitFieldStack, REG_FIELD_COORD_BITFIELD_FRAME.first, REG_FIELD_COORD_BITFIELD_FRAME.second, 1, REG_BLOCK_FIELD_WIDTH);

    //connect up new register functionality here so we can reference bitFieldStack
    connect(rbc, &RegisterBlockController::regCreated, regTable, [=]
            (const QString& name, addr_t offset, const QString& description){

        Q_UNUSED(offset);

        regTable->setRowCount(regTable->rowCount() + 1);
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
        connect(rbc, &RegisterBlockController::regDescriptionChanged, regTable, [=](const QString& new_desc){
            if (rbc->getCurrRegIdx() == curr_table_row){
                desc_item->setText(new_desc);
            }
        });

        regTable->setItem(curr_table_row, REG_TABLE_COL_NAME, name_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_OFFSET, offset_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_DESC, desc_item);

        bitFieldStack->addWidget(this->makeNewBitFieldFrame(rbc, curr_table_row));

        rbc->setCurrRegIdx(curr_table_row);

        this->setAllEnabled(regFrame, true);
    });

    setAllEnabled(regFrame, false);

    newregButton->setEnabled(true);

    return regFrame;
}

QFrame* MainWindow::makeNewBitFieldFrame(RegisterBlockController* rbc, int reg_idx){
    QFrame* bitFieldFrame = new QFrame();
    bitFieldFrame->setLineWidth(5);

    QGridLayout* bitFieldGrid = new QGridLayout();
    bitFieldFrame->setLayout(bitFieldGrid);

    QTableWidget* bitFieldTable = new QTableWidget(0, 3);
    bitFieldTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bitFieldTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bitFieldTable->setSelectionMode(QAbstractItemView::SingleSelection);
    bitFieldTable->setAutoScroll(false);
    bitFieldTable->setHorizontalHeaderItem(BITFIELD_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    bitFieldTable->setHorizontalHeaderItem(BITFIELD_TABLE_COL_RANGE, new QTableWidgetItem("Range"));
    bitFieldTable->setHorizontalHeaderItem(BITFIELD_TABLE_COL_DESC, new QTableWidgetItem("Description"));
    connect(bitFieldTable, &QTableWidget::itemClicked, rbc, [=](QTableWidgetItem* item){
        int row = bitFieldTable->row(item);
        rbc->setCurrBitFieldIdx(row);
    });

    connect(rbc, &RegisterBlockController::currBitFieldIdxChanged, bitFieldTable, &QTableWidget::selectRow);

    //this forces the highlighted row to always be the one we're editing
    connect(bitFieldTable, &QTableWidget::itemSelectionChanged, this, [=](){
        int curr_row = rbc->getCurrBitFieldIdx();
        for (QTableWidgetItem* twi : bitFieldTable->selectedItems()){
            if (bitFieldTable->row(twi) != curr_row){
                bitFieldTable->clearSelection();
                bitFieldTable->selectRow(curr_row);
                break;
            }
        }
    });

    bitFieldGrid->addWidget(bitFieldTable, BITFIELD_FIELD_COORD_BITFIELD_TABLE.first, BITFIELD_FIELD_COORD_BITFIELD_TABLE.second, 1, REG_BLOCK_FIELD_WIDTH);

    QPushButton* newBitFieldButton = new QPushButton("New Bit Field");
    newBitFieldButton->setEnabled(false);
    connect(newBitFieldButton, &QPushButton::clicked, rbc, &RegisterBlockController::makeNewBitField);
    bitFieldGrid->addWidget(newBitFieldButton, BITFIELD_FIELD_COORD_NEW_BITFIELD_BTN.first, BITFIELD_FIELD_COORD_NEW_BITFIELD_BTN.second);

    //        QPushButton* sortBitFieldsBtn = new QPushButton("Sort Bit Fields");
    //        connect(sortBitFieldsBtn, &QPushButton::clicked, rbc, [=](){
    //            if (this->checkOffsetCollisions(rbc) != OK) return;
    //            rbc->sortRegsByOffset();
    //        });
    //        g->addWidget(sortRegsButton, REG_BLOCK_FIELD_COORD_SORTREGBTN.first, REG_BLOCK_FIELD_COORD_SORTREGBTN.second);

    //connect up new register functionality here so we can reference bitFieldFrame
    connect(rbc, &RegisterBlockController::bitFieldCreated, bitFieldTable, [=]
            (const QString& name, uint32_t low_idx, uint32_t high_idx, const QString& description){

        if (rbc->getCurrRegIdx() != reg_idx) return;

        bitFieldTable->setRowCount(bitFieldTable->rowCount()+1);
        int curr_table_row = bitFieldTable->rowCount() - 1;

        QTableWidgetItem* name_item = new QTableWidgetItem(name);
        QTableWidgetItem* range_item = new QTableWidgetItem(RegisterBlockController::getBitRangeAsString(low_idx, high_idx));
        QTableWidgetItem* desc_item = new QTableWidgetItem(description);

        connect(rbc, &RegisterBlockController::bitFieldNameChanged, bitFieldTable, [=](const QString& new_name){
            if (rbc->getCurrBitFieldIdx() == curr_table_row && rbc->getCurrRegIdx() == reg_idx){
                name_item->setText(new_name);
            }
        });
        connect(rbc, &RegisterBlockController::bitFieldRangeChanged, bitFieldTable, [=](uint32_t low_idx, uint32_t high_idx){
            if (rbc->getCurrBitFieldIdx() == curr_table_row && rbc->getCurrRegIdx() == reg_idx){
                range_item->setText(RegisterBlockController::getBitRangeAsString(low_idx, high_idx));
            }
        });
        connect(rbc, &RegisterBlockController::bitFieldDescriptionChanged, bitFieldTable, [=](const QString& new_desc){
            if (rbc->getCurrBitFieldIdx() == curr_table_row && rbc->getCurrRegIdx() == reg_idx){
                desc_item->setText(new_desc);
            }
        });

        bitFieldTable->setItem(curr_table_row, BITFIELD_TABLE_COL_NAME, name_item);
        bitFieldTable->setItem(curr_table_row, BITFIELD_TABLE_COL_RANGE, range_item);
        bitFieldTable->setItem(curr_table_row, BITFIELD_TABLE_COL_DESC, desc_item);

        rbc->setCurrBitFieldIdx(curr_table_row);

        this->setAllEnabled(bitFieldFrame, true);
    });

    QLabel* bitFieldNameLabel = new QLabel("Name: ");
    bitFieldNameLabel->setEnabled(false);
    bitFieldGrid->addWidget(bitFieldNameLabel, BITFIELD_FIELD_COORD_NAME.first, BITFIELD_FIELD_COORD_NAME.second-1);

    QLineEdit* nameEdit = new QLineEdit();
    connect(nameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setBitFieldName);
    connect(rbc, &RegisterBlockController::bitFieldNameChanged, nameEdit, [=](const QString& new_name){
        if (rbc->getCurrRegIdx() == reg_idx){
            nameEdit->setText(new_name);
        }
    });
    nameEdit->setEnabled(false); //will set editable when bit field is tracked with this UI
    bitFieldGrid->addWidget(nameEdit, BITFIELD_FIELD_COORD_NAME.first, BITFIELD_FIELD_COORD_NAME.second);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    codeNameLabel->setEnabled(false);
    bitFieldGrid->addWidget(codeNameLabel, BITFIELD_FIELD_COORD_CODENAME.first, BITFIELD_FIELD_COORD_CODENAME.second-1);

    QLineEdit* codeNameEdit = new QLineEdit();
    codeNameEdit->setEnabled(false);
    codeNameEdit->setReadOnly(true);
    connect(codeNameEdit, &QLineEdit::textEdited, rbc, &RegisterBlockController::setBitFieldCodeName);
    connect(rbc, &RegisterBlockController::bitFieldCodeNameChanged, codeNameEdit, [=](const QString& new_name){
        if (rbc->getCurrRegIdx() == reg_idx){
            codeNameEdit->setText(new_name);
        }
    });
    bitFieldGrid->addWidget(codeNameEdit, BITFIELD_FIELD_COORD_CODENAME.first, BITFIELD_FIELD_COORD_CODENAME.second);

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    connect(customCNCheckBox, &QCheckBox::stateChanged, rbc, [=](int state){
        if (state == Qt::CheckState::Checked){
            rbc->setBitFieldCodeNameGeneration(false);
        } else {
            rbc->setBitFieldCodeNameGeneration(true);
        }
    });
    connect(rbc, &RegisterBlockController::bitFieldCodeNameGenerationChanged, customCNCheckBox, [=](bool gen_code_name){
        if (rbc->getCurrRegIdx() == reg_idx){
            customCNCheckBox->setChecked(!gen_code_name);
            codeNameEdit->setReadOnly(gen_code_name);
        }
    });
    customCNCheckBox->setEnabled(false); //will set editable when bit field is tracked with this UI
    bitFieldGrid->addWidget(customCNCheckBox, BITFIELD_FIELD_COORD_GEN_CODENAME.first, BITFIELD_FIELD_COORD_GEN_CODENAME.second);

    QLabel* rangeLabel = new QLabel("Range: ");
    rangeLabel->setEnabled(false);
    bitFieldGrid->addWidget(rangeLabel, BITFIELD_FIELD_COORD_RANGE_HIGH.first, BITFIELD_FIELD_COORD_RANGE_HIGH.second-1);

    QSpinBox* rangeHighEdit = new QSpinBox();
    connect(rangeHighEdit, &QSpinBox::valueChanged, rbc, [=](int new_val){
        rbc->setBitFieldRange(rbc->getCurrRegCurrBitField().low_index, new_val);
    });
    rangeHighEdit->setMinimum(0);
    rangeHighEdit->setMaximum(std::numeric_limits<int>::max());
    rangeHighEdit->setEnabled(false); //will set editable when register is tracked with this UI
    bitFieldGrid->addWidget(rangeHighEdit, BITFIELD_FIELD_COORD_RANGE_HIGH.first, BITFIELD_FIELD_COORD_RANGE_HIGH.second);

    QLabel* downToLabel = new QLabel("downto");
    downToLabel->setEnabled(false);
    bitFieldGrid->addWidget(downToLabel, BITFIELD_FIELD_COORD_RANGE_LOW.first, BITFIELD_FIELD_COORD_RANGE_LOW.second-1);

    QSpinBox* rangeLowEdit = new QSpinBox();
    connect(rangeLowEdit, &QSpinBox::valueChanged, rbc, [=](int new_val){
        rbc->setBitFieldRange(new_val, rbc->getCurrRegCurrBitField().high_index);
    });
    rangeLowEdit->setMinimum(0);
    rangeLowEdit->setMaximum(std::numeric_limits<int>::max());
    rangeLowEdit->setEnabled(false); //will set editable when register is tracked with this UI
    bitFieldGrid->addWidget(rangeLowEdit, BITFIELD_FIELD_COORD_RANGE_LOW.first, BITFIELD_FIELD_COORD_RANGE_LOW.second);

    //adjust spinbox ranges in response to range changing
    connect(rbc, &RegisterBlockController::bitFieldRangeChanged, this, [=](uint32_t low_idx, uint32_t high_idx){
        if (rbc->getCurrRegIdx() == reg_idx){
            if ((uint32_t)rangeHighEdit->minimum() != low_idx){
                rangeHighEdit->setMinimum(low_idx);
            }
            if ((uint32_t)rangeLowEdit->maximum() != high_idx){
                rangeLowEdit->setMaximum(high_idx);
            }
            if (high_idx != (uint32_t)rangeHighEdit->value()){
                rangeHighEdit->setValue(high_idx);
            }
            if (low_idx != (uint32_t)rangeLowEdit->value()){
                rangeLowEdit->setValue(low_idx);
            }
        }
    });

    //adjust high range spinbox range maximum in response to register bit length
    connect(rbc, &RegisterBlockController::regBitLenChanged, this, [=](uint32_t new_len){
        if (rbc->getCurrRegIdx() == reg_idx && (uint32_t)rangeHighEdit->maximum() != new_len-1){
            rangeHighEdit->setMaximum(new_len-1);
        }
    });

    QLabel* descLabel = new QLabel("Description: ");
    bitFieldGrid->addWidget(descLabel, BITFIELD_FIELD_COORD_DESC_LABEL.first, BITFIELD_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    connect(descEdit, &QTextEdit::textChanged, rbc, [=](){
        rbc->setBitFieldDescription(descEdit->toPlainText());
    });
    connect(rbc, &RegisterBlockController::bitFieldDescriptionChanged, descEdit, [=](const QString& new_desc){
        if (rbc->getCurrRegIdx() == reg_idx && descEdit->toPlainText().compare(new_desc)){
            descEdit->setText(new_desc);
        }
    });
    bitFieldGrid->addWidget(descEdit, BITFIELD_FIELD_COORD_DESC.first, BITFIELD_FIELD_COORD_DESC.second, 1, BITFIELD_FIELD_WIDTH);

    //disable all bitfield stuff till we start tracking a bitfield
    setAllEnabled(bitFieldFrame, false);

    newBitFieldButton->setEnabled(true);

    return bitFieldFrame;
}

void MainWindow::setAllEnabled(QWidget* parent, bool enabled){
    for (QObject* qo : parent->children()){
        if (QWidget* w = qobject_cast<QWidget*>(qo)){
            w->setEnabled(enabled);
        }
    }
}

