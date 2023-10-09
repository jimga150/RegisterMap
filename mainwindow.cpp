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
#define REG_BLOCK_FIELD_COORD_CODENAME      (std::pair<int, int>(1, 1))
#define REG_BLOCK_FIELD_COORD_GEN_CODENAME  (std::pair<int, int>(2, 0))
#define REG_BLOCK_FIELD_COORD_SIZE          (std::pair<int, int>(3, 1))
#define REG_BLOCK_FIELD_COORD_DESC_LABEL    (std::pair<int, int>(4, 0))
#define REG_BLOCK_FIELD_COORD_DESC          (std::pair<int, int>(5, 0))
#define REG_BLOCK_FIELD_COORD_NEWREGBTN     (std::pair<int, int>(6, 0))
#define REG_BLOCK_FIELD_COORD_SORTREGBTN    (std::pair<int, int>(6, 1))
#define REG_BLOCK_FIELD_COORD_REGTABLE      (std::pair<int, int>(7, 0))
#define REG_BLOCK_FIELD_COORD_REGFRAME      (std::pair<int, int>(0, 2))

#define REG_BLOCK_FIELD_WIDTH   (3)
#define REG_BLOCK_FIELD_HEIGHT  (8)

#define REG_FIELD_COORD_NAME                (std::pair<int, int>(0, 1))
#define REG_FIELD_COORD_CODENAME            (std::pair<int, int>(1, 1))
#define REG_FIELD_COORD_GEN_CODENAME        (std::pair<int, int>(2, 0))
#define REG_FIELD_COORD_OFFSET              (std::pair<int, int>(3, 1))
#define REG_FIELD_COORD_BITLEN              (std::pair<int, int>(4, 1))
#define REG_FIELD_COORD_BYTELEN             (std::pair<int, int>(5, 1))
#define REG_FIELD_COORD_DESC_LABEL          (std::pair<int, int>(6, 0))
#define REG_FIELD_COORD_DESC                (std::pair<int, int>(7, 0))
#define REG_FIELD_COORD_NEW_BITFIELD_BTN    (std::pair<int, int>(8, 0))
#define REG_FIELD_COORD_BITFIELD_TABLE      (std::pair<int, int>(9, 0))
#define REG_FIELD_COORD_BITFIELD_FRAME      (std::pair<int, int>(10, 0))

#define REG_FIELD_WIDTH   (2)
#define REG_FIELD_HEIGHT  (11)

#define REG_TABLE_COL_NAME      (0)
#define REG_TABLE_COL_OFFSET    (1)
#define REG_TABLE_COL_DESC      (2)
#define REG_TABLE_COL_OFFSETINT (3)
#define REG_TABLE_COL_MAX       REG_TABLE_COL_OFFSETINT

#define BITFIELD_FIELD_COORD_NAME               (std::pair<int, int>(0, 2))
#define BITFIELD_FIELD_COORD_CODENAME           (std::pair<int, int>(1, 2))
#define BITFIELD_FIELD_COORD_GEN_CODENAME       (std::pair<int, int>(2, 1))
#define BITFIELD_FIELD_COORD_RANGE_HIGH         (std::pair<int, int>(3, 2))
#define BITFIELD_FIELD_COORD_RANGE_LOW          (std::pair<int, int>(4, 2))
#define BITFIELD_FIELD_COORD_DESC_LABEL         (std::pair<int, int>(5, 1))
#define BITFIELD_FIELD_COORD_DESC               (std::pair<int, int>(6, 1))

#define BITFIELD_FIELD_WIDTH   (3)
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
            if (rbc->getRegControllerAt(i)->getOffset() == rbc->getRegControllerAt(j)->getOffset()){
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
            warn_msg += rbc->getRegControllerAt(i)->getName();
            warn_msg += "\tOffset: ";
            warn_msg += rbc->getRegControllerAt(i)->getOffsetAsString();
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
            if (!(rbc->getRegControllerAt(i)->getCodeName().compare(rbc->getRegControllerAt(j)->getCodeName()))){
                if (!colliding_reg_idxs.contains(i)) colliding_reg_idxs.push_back(i);
                if (!colliding_reg_idxs.contains(j)) colliding_reg_idxs.push_back(j);
            }
        }
        if (rbc->getRegControllerAt(i)->getCodeName().length() == 0){
            if (!empty_reg_idxs.contains(i)) empty_reg_idxs.push_back(i);
        }
    }

    if (colliding_reg_idxs.length() > 0){
        QString warn_msg = "One or more Registers in " + rbc->getName() + " have the same Source-Friendly name.";
        for (int i : colliding_reg_idxs){
            warn_msg += "\nName: ";
            warn_msg += rbc->getRegControllerAt(i)->getName();
            warn_msg += "\tSource-Friendly Name: ";
            warn_msg += rbc->getCodeName();
        }
        QMessageBox::warning(this, "Validation Failed: " + rbc->getName(), warn_msg);
        result = ERROR;
    }

    if (empty_reg_idxs.length() > 0){
        QString warn_msg = "One or more Registers in " + rbc->getName() + " have no Source-Friendly name.";
        for (int i : empty_reg_idxs){
            warn_msg += "\nName: ";
            warn_msg += rbc->getRegControllerAt(i)->getName();
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

        for (uint i = 0; i < p->getNumRegs(); ++i){

            toml_value_t bitfield_array;

            for (uint b = 0; b < p->getRegControllerAt(i)->getNumBitFields(); ++b){

                BitFieldController* bfc = p->getRegControllerAt(i)->getBitFieldControllerAt(b);

                toml_value_t bitfield_record{
                    {BitFieldController::name_key, bfc->getName().toStdString()},
                    {BitFieldController::codename_key, bfc->getCodeName().toStdString()},
                    {BitFieldController::codenamegen_key,
                        bfc->getCodeNameGeneration() ? "true" : "false"},
                    {BitFieldController::desc_key, bfc->getDescription().toStdString()},
                    {BitFieldController::high_idx_key, bfc->getHighIdx()},
                    {BitFieldController::low_idx_key, bfc->getLowIdx()}
                };

                toml_id = std::to_string(bfc->getLowIdx()) + "_" + bfc->getCodeName().toStdString();
                bitfield_array[toml_id] = bitfield_record;
            }

            RegisterController* rc = p->getRegControllerAt(i);

//            printf("Collecting register %s (0x%x)\n", p->getRegName(i).toUtf8().constData(), p->getRegOffset(i));
            toml_value_t reg_record{
                {RegisterController::name_key, rc->getName().toStdString()},
                {RegisterController::codename_key, rc->getCodeName().toStdString()},
                {RegisterController::codenamegen_key, rc->getCodeNameGeneration() ? "true" : "false"},
                {RegisterController::offset_key, rc->getOffset()},
                {RegisterController::bitlen_key, rc->getBitLen()},
                {RegisterController::desc_key, rc->getDescription().toStdString()}
            };

            //only add bit field array if any exist to add, adding empty arrays is bad in TOML
            if (rc->getNumBitFields() > 0){
                reg_record.as_table()[RegisterController::bitfields_key] = bitfield_array;
            }

            toml_id = std::to_string(rc->getOffset()) + "_" + rc->getCodeName().toStdString();
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
        name = toml::find<std::string>(reg_table, RegisterController::name_key);
    } catch (std::out_of_range& e){
        name = table_key;
    }

    std::string codename;
    try {
        codename = toml::find<std::string>(reg_table, RegisterController::codename_key);
    } catch (std::out_of_range& e){
        codename = table_key;
    }

    bool gen_codename;
    try {
        gen_codename = toml::find<std::string>(reg_table, RegisterController::codenamegen_key).compare("false");
    } catch (std::out_of_range& e){
        gen_codename = true;
    }

    addr_t offset;
    try {
        offset = toml::find<addr_t>(reg_table, RegisterController::offset_key);
    } catch (std::out_of_range& e){
        //TODO: this should be an error.
        offset = 0;
    }

    uint32_t bit_len;
    try {
        bit_len = toml::find<uint32_t>(reg_table, RegisterController::bitlen_key);
    } catch (std::out_of_range& e){
        //TODO: this should be an error.
        bit_len = 8;
    }

    std::string description;
    try {
        description = toml::find<std::string>(reg_table, RegisterController::desc_key);
    } catch (std::out_of_range& e){
        description = "";
    }

    rbc->makeNewReg();
    int new_reg_idx = rbc->getNumRegs() - 1;
    rbc->setCurrRegIdx(new_reg_idx);

    RegisterController* rc = rbc->getCurrRegController();

    rc->setName(name.c_str());
    rc->setCodeNameGeneration(gen_codename);
    rc->setCodeName(codename.c_str());
    rc->setOffset(offset);
    rc->setBitLen(bit_len);
    rc->setDescription(description.c_str());

    toml_value_t bitfield_table;
    try {
        bitfield_table = toml::find(reg_table, RegisterController::bitfields_key);
    } catch (std::out_of_range& e){
        //no bitfields? no problem! Just dont make any
    }

    if (!bitfield_table.is_table()) return;

    for (std::pair<const std::string, toml_value_t>& kv : bitfield_table.as_table()){
        const std::string key = kv.first;
        toml_value_t val = kv.second;

        this->loadBitField(val, key, rc);
    }
}

void MainWindow::loadBitField(toml_value_t bitfield_table, std::string table_key, RegisterController* rc)
{
    std::string name;
    try {
        name = toml::find<std::string>(bitfield_table, BitFieldController::name_key);
    } catch (std::out_of_range& e){
        name = table_key;
    }

    std::string codename;
    try {
        codename = toml::find<std::string>(bitfield_table, BitFieldController::codename_key);
    } catch (std::out_of_range& e){
        codename = table_key;
    }

    bool gen_codename;
    try {
        gen_codename = toml::find<std::string>(bitfield_table, BitFieldController::codenamegen_key).compare("false");
    } catch (std::out_of_range& e){
        gen_codename = true;
    }

    std::string description;
    try {
        description = toml::find<std::string>(bitfield_table, BitFieldController::desc_key);
    } catch (std::out_of_range& e){
        description = "";
    }

    uint32_t high_idx;
    try {
        high_idx = toml::find<uint32_t>(bitfield_table, BitFieldController::high_idx_key);
    } catch (std::out_of_range& e){
        high_idx = 0;
    }

    uint32_t low_idx;
    try {
        low_idx = toml::find<uint32_t>(bitfield_table, BitFieldController::low_idx_key);
    } catch (std::out_of_range& e){
        low_idx = 0;
    }

    if (low_idx > high_idx){
        uint32_t new_low = high_idx;
        printf(
            "Adjusting invalid bit range: %d:%d -> %d:%d\n",
            low_idx, high_idx, new_low, high_idx
            );
        low_idx = new_low;
    }

    rc->makeNewBitField();
    uint new_bitfield_idx = rc->getNumBitFields() - 1;
    rc->setBitFieldIdx(new_bitfield_idx);

    BitFieldController* bfc = rc->getCurrBitFieldController();

    bfc->setName(name.c_str());
    bfc->setCodeNameGeneration(gen_codename);
    bfc->setCodeName(codename.c_str());
    bfc->setDescription(description.c_str());
    bfc->setRange(low_idx, high_idx);
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
    g->addWidget(customCNCheckBox, REG_BLOCK_FIELD_COORD_GEN_CODENAME.first, REG_BLOCK_FIELD_COORD_GEN_CODENAME.second, 1, 2);

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
    g->addWidget(descEdit, REG_BLOCK_FIELD_COORD_DESC.first, REG_BLOCK_FIELD_COORD_DESC.second, 1, REG_BLOCK_FIELD_WIDTH-1);

    QPushButton* newregButton = new QPushButton("New Register");
    connect(newregButton, &QPushButton::clicked, rbc, &RegisterBlockController::makeNewReg);
    g->addWidget(newregButton, REG_BLOCK_FIELD_COORD_NEWREGBTN.first, REG_BLOCK_FIELD_COORD_NEWREGBTN.second);

    QPushButton* sortRegsButton = new QPushButton("Sort Registers by Offset");
    connect(sortRegsButton, &QPushButton::clicked, rbc, [=](){
        if (this->checkOffsetCollisions(rbc) != OK) return;
        rbc->sortRegsByOffset();
    });
    g->addWidget(sortRegsButton, REG_BLOCK_FIELD_COORD_SORTREGBTN.first, REG_BLOCK_FIELD_COORD_SORTREGBTN.second);

    QTableWidget* regTable = new QTableWidget(0, REG_TABLE_COL_MAX+1);
    regTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    regTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    regTable->setSelectionMode(QAbstractItemView::SingleSelection);
    regTable->setAutoScroll(false);
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_OFFSET, new QTableWidgetItem("Offset"));
    regTable->setHorizontalHeaderItem(REG_TABLE_COL_DESC, new QTableWidgetItem("Description"));
    regTable->setColumnHidden(REG_TABLE_COL_OFFSETINT, true);
    connect(regTable, &QTableWidget::itemClicked, rbc, [=](QTableWidgetItem* item){
        int row = regTable->row(item);
        rbc->setCurrRegIdx(row);
    });

    connect(rbc, &RegisterBlockController::currRegIdxChanged, regTable, &QTableWidget::selectRow);

    connect(rbc, &RegisterBlockController::regIdxsReassigned, regTable, [=](){
        regTable->sortItems(REG_TABLE_COL_OFFSETINT, Qt::SortOrder::AscendingOrder);
    });

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

    g->addWidget(regTable, REG_BLOCK_FIELD_COORD_REGTABLE.first, REG_BLOCK_FIELD_COORD_REGTABLE.second, 1, REG_BLOCK_FIELD_COORD_REGFRAME.second);
//    g->setColumnStretch(REG_BLOCK_FIELD_COORD_REGTABLE.second, 1);
    g->setRowStretch(REG_BLOCK_FIELD_COORD_REGTABLE.first, 1);

    QFrame* regFrame = this->makeNewRegFrame(rbc);

    g->addWidget(
        regFrame,
        REG_BLOCK_FIELD_COORD_REGFRAME.first,
        REG_BLOCK_FIELD_COORD_REGFRAME.second,
        REG_BLOCK_FIELD_HEIGHT - REG_BLOCK_FIELD_COORD_REGFRAME.first,
        REG_BLOCK_FIELD_WIDTH - REG_BLOCK_FIELD_COORD_REGFRAME.second
        );
//    g->setColumnStretch(REG_BLOCK_FIELD_COORD_REGFRAME.second, 1);

    //connect up new register functionality here so we can reference regFrame
    connect(rbc, &RegisterBlockController::regCreated, regTable, [=](RegisterController* rc){

        regTable->setRowCount(regTable->rowCount() + 1);
        int curr_table_row = regTable->rowCount() - 1;

        QTableWidgetItem* name_item = new QTableWidgetItem(rc->getName());
        QTableWidgetItem* offset_item = new QTableWidgetItem(rc->getOffsetAsString());
        QTableWidgetItem* desc_item = new QTableWidgetItem(rc->getDescription());
        QTableWidgetItem* offset_int_item = new QTableWidgetItem(this->getSortableString(rc->getOffset()));

        connect(rc, &RegisterController::nameChanged, regTable, [=](const QString& new_name){
            name_item->setText(new_name);
        });
        connect(rc, &RegisterController::offsetChanged, regTable, [=](addr_t new_offset){
            offset_item->setText(rc->getOffsetAsString());
            offset_int_item->setText(this->getSortableString(new_offset));
        });
        connect(rc, &RegisterController::descriptionChanged, regTable, [=](const QString& new_name){
            desc_item->setText(new_name);
        });

        regTable->setItem(curr_table_row, REG_TABLE_COL_NAME, name_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_OFFSET, offset_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_DESC, desc_item);
        regTable->setItem(curr_table_row, REG_TABLE_COL_OFFSETINT, offset_int_item);

        rbc->setCurrRegIdx(curr_table_row);

        this->setAllEnabled(regFrame, true);
    });

    for (int i = 0; i < g->rowCount(); ++i){
        printf("g row %d: %d\n", i, g->rowStretch(i));
    }

//    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
//    g->addItem(spacer, REG_BLOCK_FIELD_COORD_SPACER.first, REG_BLOCK_FIELD_COORD_SPACER.second);

    //notify of any actual data changes
    connect(rbc, &RegisterBlockController::changeMade, this, &MainWindow::changeMade);

    return w;
}

QFrame* MainWindow::makeNewRegFrame(RegisterBlockController* rbc){

    QFrame* regFrame = new QFrame();
    regFrame->setLineWidth(5);
//    regFrame->setStyleSheet("background-color: rgb(0,150,0)");

    QGridLayout* reggrid = new QGridLayout();
    regFrame->setLayout(reggrid);

    QLabel* regNameLabel = new QLabel("Name: ");
    reggrid->addWidget(regNameLabel, REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second-1);

    QLineEdit* nameEdit = new QLineEdit();
    reggrid->addWidget(nameEdit, REG_FIELD_COORD_NAME.first, REG_FIELD_COORD_NAME.second);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    reggrid->addWidget(codeNameLabel, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second-1);

    QLineEdit* codeNameEdit = new QLineEdit();
    codeNameEdit->setReadOnly(true);
    reggrid->addWidget(codeNameEdit, REG_FIELD_COORD_CODENAME.first, REG_FIELD_COORD_CODENAME.second);

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    reggrid->addWidget(customCNCheckBox, REG_FIELD_COORD_GEN_CODENAME.first, REG_FIELD_COORD_GEN_CODENAME.second, 1, 2);

    QLabel* offsetLabel = new QLabel("Offset (in addrs): ");
    reggrid->addWidget(offsetLabel, REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second-1);

    QSpinBox* offsetEdit = new QSpinBox();
    offsetEdit->setMinimum(0);
    offsetEdit->setMaximum(std::numeric_limits<int>::max());
    offsetEdit->setDisplayIntegerBase(16);
    offsetEdit->setPrefix("0x");
    reggrid->addWidget(offsetEdit, REG_FIELD_COORD_OFFSET.first, REG_FIELD_COORD_OFFSET.second);

    QLabel* bitLenLabel = new QLabel("Size (bits): ");
    reggrid->addWidget(bitLenLabel, REG_FIELD_COORD_BITLEN.first, REG_FIELD_COORD_BITLEN.second-1);

    QSpinBox* bitLenEdit = new QSpinBox();
    bitLenEdit->setValue(1);
    bitLenEdit->setMinimum(1);
    bitLenEdit->setMaximum(std::numeric_limits<int>::max());
    reggrid->addWidget(bitLenEdit, REG_FIELD_COORD_BITLEN.first, REG_FIELD_COORD_BITLEN.second);

    QLabel* byteLenLabel = new QLabel("Size (bytes): ");
    reggrid->addWidget(byteLenLabel, REG_FIELD_COORD_BYTELEN.first, REG_FIELD_COORD_BYTELEN.second-1);

    QSpinBox* byteLenEdit = new QSpinBox();
    byteLenEdit->setValue(1);
    byteLenEdit->setMinimum(1);
    byteLenEdit->setMaximum(std::numeric_limits<int>::max());
    reggrid->addWidget(byteLenEdit, REG_FIELD_COORD_BYTELEN.first, REG_FIELD_COORD_BYTELEN.second);

    QLabel* descLabel = new QLabel("Description: ");
    reggrid->addWidget(descLabel, REG_FIELD_COORD_DESC_LABEL.first, REG_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    reggrid->addWidget(descEdit, REG_FIELD_COORD_DESC.first, REG_FIELD_COORD_DESC.second, 1, REG_FIELD_WIDTH);

    QPushButton* newBitFieldButton = new QPushButton("New Bit Field");
    reggrid->addWidget(newBitFieldButton, REG_FIELD_COORD_NEW_BITFIELD_BTN.first, REG_FIELD_COORD_NEW_BITFIELD_BTN.second);

    //        QPushButton* sortBitFieldsBtn = new QPushButton("Sort Bit Fields");
    //        connect(sortBitFieldsBtn, &QPushButton::clicked, rbc, [=](){
    //            if (this->checkOffsetCollisions(rbc) != OK) return;
    //            rbc->sortRegsByOffset();
    //        });
    //        g->addWidget(sortRegsButton, REG_BLOCK_FIELD_COORD_SORTREGBTN.first, REG_BLOCK_FIELD_COORD_SORTREGBTN.second);

    QTableWidget* bitFieldTable = new QTableWidget(0, 3);
    bitFieldTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bitFieldTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bitFieldTable->setSelectionMode(QAbstractItemView::SingleSelection);
    bitFieldTable->setAutoScroll(false);
    bitFieldTable->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    bitFieldTable->setHorizontalHeaderItem(BITFIELD_TABLE_COL_NAME, new QTableWidgetItem("Name"));
    bitFieldTable->setHorizontalHeaderItem(BITFIELD_TABLE_COL_RANGE, new QTableWidgetItem("Range"));
    bitFieldTable->setHorizontalHeaderItem(BITFIELD_TABLE_COL_DESC, new QTableWidgetItem("Description"));
    connect(bitFieldTable, &QTableWidget::itemClicked, rbc, [=](QTableWidgetItem* item){
        int row = bitFieldTable->row(item);
        rbc->getCurrRegController()->setBitFieldIdx(row);
    });

    connect(rbc, &RegisterBlockController::currBitFieldIdxChanged, bitFieldTable, &QTableWidget::selectRow);

    //this forces the highlighted row to always be the one we're editing
    connect(bitFieldTable, &QTableWidget::itemSelectionChanged, this, [=](){
        int curr_row = rbc->getCurrRegController()->getCurrBitFieldIdx();
        for (QTableWidgetItem* twi : bitFieldTable->selectedItems()){
            if (bitFieldTable->row(twi) != curr_row){
                bitFieldTable->clearSelection();
                bitFieldTable->selectRow(curr_row);
                break;
            }
        }
    });

    reggrid->addWidget(bitFieldTable, REG_FIELD_COORD_BITFIELD_TABLE.first, REG_FIELD_COORD_BITFIELD_TABLE.second, 1, REG_FIELD_WIDTH);
//    reggrid->setColumnStretch(REG_FIELD_COORD_BITFIELD_TABLE.second, 1);
    reggrid->setRowStretch(REG_FIELD_COORD_BITFIELD_TABLE.first, 1);

    QFrame* bitFieldFrame = this->makeNewBitFieldFrame(rbc);
    reggrid->addWidget(bitFieldFrame, REG_FIELD_COORD_BITFIELD_FRAME.first, REG_FIELD_COORD_BITFIELD_FRAME.second, 1, REG_FIELD_WIDTH);

    connect(rbc, &RegisterBlockController::currRegIdxChanged, this, [=](int new_idx){

        //disconnect everything between the register frame and the previously selected register
        for (QMetaObject::Connection& c : this->reg_ui_connections){
            if (!disconnect(c)){
                fprintf(stderr, "Failed to disconnect a Register <-> UI connection!\n");
            }
        }
        this->reg_ui_connections.clear();

        //connect up the new register
        RegisterController* rc = rbc->getRegControllerAt(new_idx);

        this->reg_ui_connections.push_back(
            connect(newBitFieldButton, &QPushButton::clicked, rc, &RegisterController::makeNewBitField)
        );

        this->reg_ui_connections.push_back(
            connect(nameEdit, &QLineEdit::textEdited, rc, &RegisterController::setName)
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::nameChanged, nameEdit, &QLineEdit::setText)
        );
        emit rc->nameChanged(rc->getName());

        this->reg_ui_connections.push_back(
            connect(codeNameEdit, &QLineEdit::textEdited, rc, &RegisterController::setCodeName)
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::codeNameChanged, codeNameEdit, &QLineEdit::setText)
        );
        emit rc->codeNameChanged(rc->getCodeName());

        this->reg_ui_connections.push_back(
            connect(customCNCheckBox, &QCheckBox::stateChanged, rc, [=](int state){
                if (state == Qt::CheckState::Checked){
                    rc->setCodeNameGeneration(false);
                } else {
                    rc->setCodeNameGeneration(true);
                }
            })
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::codeNameGenerationChanged, customCNCheckBox,
                    [=](bool gen_code_name){
                customCNCheckBox->setChecked(!gen_code_name);
                codeNameEdit->setReadOnly(gen_code_name);
            })
        );
        emit rc->codeNameGenerationChanged(rc->getCodeNameGeneration());

        this->reg_ui_connections.push_back(
            connect(offsetEdit, &QSpinBox::valueChanged, rc, &RegisterController::setOffset)
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::offsetChanged, offsetEdit, [=](addr_t new_offset){
                if (new_offset != (addr_t)offsetEdit->value()){
                    offsetEdit->setValue(new_offset);
                }
            })
        );
        emit rc->offsetChanged(rc->getOffset());

        this->reg_ui_connections.push_back(
            connect(bitLenEdit, &QSpinBox::valueChanged, rc, [=](int new_val){
                if (bitLenEdit->hasFocus()){
                    rc->setBitLen(new_val);
                }
            })
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::bitLenChanged, bitLenEdit, [=](uint32_t new_bitlen){
                if (new_bitlen != (uint32_t)bitLenEdit->value()){
                    bitLenEdit->setValue(new_bitlen);
                }
            })
        );

        this->reg_ui_connections.push_back(
            connect(byteLenEdit, &QSpinBox::valueChanged, rc, [=](int new_val){
                if (byteLenEdit->hasFocus()){
                    rc->setBitLen(new_val*BITS_PER_BYTE);
                }
            })
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::bitLenChanged, byteLenEdit, [=](uint32_t new_bitlen){
                uint32_t new_bytelen = (uint32_t)ceil((float)new_bitlen*1.0/BITS_PER_BYTE);
                if (new_bytelen != (uint32_t)byteLenEdit->value()){
                    byteLenEdit->setValue(new_bytelen);
                }
            })
        );
        emit rc->bitLenChanged(rc->getBitLen());

        this->reg_ui_connections.push_back(
            connect(descEdit, &QTextEdit::textChanged, rc, [=](){
                rc->setDescription(descEdit->toPlainText());
            })
        );
        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::descriptionChanged, descEdit, [=](const QString& new_desc){
                if (descEdit->toPlainText().compare(new_desc)){
                    descEdit->setText(new_desc);
                }
            })
        );
        emit rc->descriptionChanged(rc->getDescription());

        this->reg_ui_connections.push_back(
            connect(rc, &RegisterController::bitFieldCreated, bitFieldTable, [=](BitFieldController* bfc){

                bitFieldTable->setRowCount(bitFieldTable->rowCount()+1);
                int curr_table_row = bitFieldTable->rowCount() - 1;

                QTableWidgetItem* name_item = new QTableWidgetItem(bfc->getName());
                QTableWidgetItem* range_item = new QTableWidgetItem(bfc->getBitRangeAsString());
                QTableWidgetItem* desc_item = new QTableWidgetItem(bfc->getDescription());

                this->reg_ui_connections.push_back(
                    connect(bfc, &BitFieldController::nameChanged, bitFieldTable, [=](const QString& new_name){
                        name_item->setText(new_name);
                    })
                );
                this->reg_ui_connections.push_back(
                    connect(bfc, &BitFieldController::rangeStrChanged, bitFieldTable, [=](const QString& new_name){
                        range_item->setText(new_name);
                    })
                );
                this->reg_ui_connections.push_back(
                    connect(bfc, &BitFieldController::descriptionChanged, bitFieldTable, [=](const QString& new_name){
                        desc_item->setText(new_name);
                    })
                );

                bitFieldTable->setItem(curr_table_row, BITFIELD_TABLE_COL_NAME, name_item);
                bitFieldTable->setItem(curr_table_row, BITFIELD_TABLE_COL_RANGE, range_item);
                bitFieldTable->setItem(curr_table_row, BITFIELD_TABLE_COL_DESC, desc_item);

                this->setAllEnabled(bitFieldFrame, true);

                rc->setBitFieldIdx(curr_table_row);
            })
        );

        int curr_bf_idx = rc->getCurrBitFieldIdx();

        //run the bitfield creation signal for each pre=existing bitfield in the register to get the table to populate
        for (int r = bitFieldTable->rowCount()-1; r >= 0; --r){
            bitFieldTable->removeRow(r);
        }
        for (uint b = 0; b < rc->getNumBitFields(); ++b){
            BitFieldController* bfc = rc->getBitFieldControllerAt(b);
            emit rc->bitFieldCreated(bfc);
        }
        rc->setBitFieldIdx(curr_bf_idx);
    });

    setAllEnabled(regFrame, false);

    for (int i = 0; i < reggrid->rowCount(); ++i){
        printf("reggrid row %d: %d\n", i, reggrid->rowStretch(i));
    }

    return regFrame;
}

QFrame* MainWindow::makeNewBitFieldFrame(RegisterBlockController* rbc){

    QFrame* bitFieldFrame = new QFrame();
    bitFieldFrame->setLineWidth(5);
//    bitFieldFrame->setStyleSheet("background-color: rgb(150,0,0)");

    QGridLayout* bitFieldGrid = new QGridLayout();
    bitFieldFrame->setLayout(bitFieldGrid);

    QLabel* bitFieldNameLabel = new QLabel("Name: ");
    bitFieldGrid->addWidget(bitFieldNameLabel, BITFIELD_FIELD_COORD_NAME.first, BITFIELD_FIELD_COORD_NAME.second-1);

    QLineEdit* nameEdit = new QLineEdit();
    bitFieldGrid->addWidget(nameEdit, BITFIELD_FIELD_COORD_NAME.first, BITFIELD_FIELD_COORD_NAME.second);

    QLabel* codeNameLabel = new QLabel("Source-Friendly Name: ");
    bitFieldGrid->addWidget(codeNameLabel, BITFIELD_FIELD_COORD_CODENAME.first, BITFIELD_FIELD_COORD_CODENAME.second-1);

    QLineEdit* codeNameEdit = new QLineEdit();
    codeNameEdit->setReadOnly(true);
    bitFieldGrid->addWidget(codeNameEdit, BITFIELD_FIELD_COORD_CODENAME.first, BITFIELD_FIELD_COORD_CODENAME.second);

    QCheckBox* customCNCheckBox = new QCheckBox("Specify custom Source-Friendly Name");
    bitFieldGrid->addWidget(customCNCheckBox, BITFIELD_FIELD_COORD_GEN_CODENAME.first, BITFIELD_FIELD_COORD_GEN_CODENAME.second, 1, BITFIELD_FIELD_WIDTH-1);

    QLabel* rangeLabel = new QLabel("Range: ");
    bitFieldGrid->addWidget(rangeLabel, BITFIELD_FIELD_COORD_RANGE_HIGH.first, BITFIELD_FIELD_COORD_RANGE_HIGH.second-1);

    QSpinBox* rangeHighEdit = new QSpinBox();
    rangeHighEdit->setMinimum(0);
    rangeHighEdit->setMaximum(std::numeric_limits<int>::max());
    bitFieldGrid->addWidget(rangeHighEdit, BITFIELD_FIELD_COORD_RANGE_HIGH.first, BITFIELD_FIELD_COORD_RANGE_HIGH.second);

    QLabel* downToLabel = new QLabel("downto");
    bitFieldGrid->addWidget(downToLabel, BITFIELD_FIELD_COORD_RANGE_LOW.first, BITFIELD_FIELD_COORD_RANGE_LOW.second-1);

    QSpinBox* rangeLowEdit = new QSpinBox();
    rangeLowEdit->setMinimum(0);
    rangeLowEdit->setMaximum(std::numeric_limits<int>::max());
    bitFieldGrid->addWidget(rangeLowEdit, BITFIELD_FIELD_COORD_RANGE_LOW.first, BITFIELD_FIELD_COORD_RANGE_LOW.second);

    QLabel* descLabel = new QLabel("Description: ");
    bitFieldGrid->addWidget(descLabel, BITFIELD_FIELD_COORD_DESC_LABEL.first, BITFIELD_FIELD_COORD_DESC_LABEL.second);

    QTextEdit* descEdit = new QTextEdit();
    bitFieldGrid->addWidget(descEdit, BITFIELD_FIELD_COORD_DESC.first, BITFIELD_FIELD_COORD_DESC.second, 1, BITFIELD_FIELD_WIDTH-1);

    connect(rbc, &RegisterBlockController::currBitFieldIdxChanged, this, [=](int new_bf_idx){
        //disconnect everything between the register frame and the previously selected register
        for (QMetaObject::Connection& c : this->bitfield_ui_connections){
            if (!disconnect(c)){
                fprintf(stderr, "Failed to disconnect a Bit Field <-> UI connection!\n");
            }
        }
        this->bitfield_ui_connections.clear();

        //connect up the new register
        RegisterController* rc = rbc->getCurrRegController();
        BitFieldController* bfc = rc->getBitFieldControllerAt(new_bf_idx);

        this->bitfield_ui_connections.push_back(
            connect(nameEdit, &QLineEdit::textEdited, bfc, &BitFieldController::setName)
        );
        this->bitfield_ui_connections.push_back(
            connect(bfc, &BitFieldController::nameChanged, nameEdit, &QLineEdit::setText)
        );
        emit bfc->nameChanged(bfc->getName());

        this->bitfield_ui_connections.push_back(
            connect(codeNameEdit, &QLineEdit::textEdited, bfc, &BitFieldController::setCodeName)
        );
        this->bitfield_ui_connections.push_back(
            connect(bfc, &BitFieldController::codeNameChanged, codeNameEdit, &QLineEdit::setText)
        );
        emit bfc->codeNameChanged(bfc->getCodeName());

        this->bitfield_ui_connections.push_back(
            connect(customCNCheckBox, &QCheckBox::stateChanged, rbc, [=](int state){
                if (state == Qt::CheckState::Checked){
                    bfc->setCodeNameGeneration(false);
                } else {
                    bfc->setCodeNameGeneration(true);
                }
            })
        );
        this->bitfield_ui_connections.push_back(
            connect(bfc, &BitFieldController::codeNameGenerationChanged, customCNCheckBox, [=](bool gen_code_name){
                customCNCheckBox->setChecked(!gen_code_name);
                codeNameEdit->setReadOnly(gen_code_name);
            })
        );
        emit bfc->codeNameGenerationChanged(bfc->getCodeNameGeneration());

        this->bitfield_ui_connections.push_back(
            connect(rangeHighEdit, &QSpinBox::valueChanged, bfc, [=](int new_val){
                bfc->setRange(bfc->getLowIdx(), new_val);
            })
        );
        this->bitfield_ui_connections.push_back(
            connect(rangeLowEdit, &QSpinBox::valueChanged, bfc, [=](int new_val){
                bfc->setRange(new_val, bfc->getHighIdx());
            })
        );
        this->bitfield_ui_connections.push_back(
            //adjust spinbox ranges in response to range changing
            connect(bfc, &BitFieldController::rangeChanged, this, [=](uint32_t low_idx, uint32_t high_idx){
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
            })
        );
        emit bfc->rangeChanged(bfc->getLowIdx(), bfc->getHighIdx());

        this->bitfield_ui_connections.push_back(
            //adjust high range spinbox range maximum in response to register bit length
            connect(rc, &RegisterController::bitLenChanged, this, [=](uint32_t new_len){
                if ((uint32_t)rangeHighEdit->maximum() != new_len-1){
                    rangeHighEdit->setMaximum(new_len-1);
                }
            })
        );

        this->bitfield_ui_connections.push_back(
            connect(descEdit, &QTextEdit::textChanged, bfc, [=](){
                bfc->setDescription(descEdit->toPlainText());
            })
        );
        this->bitfield_ui_connections.push_back(
            connect(bfc, &BitFieldController::descriptionChanged, descEdit, [=](const QString& new_desc){
                if (descEdit->toPlainText().compare(new_desc)){
                    descEdit->setText(new_desc);
                }
            })
        );
        emit bfc->descriptionChanged(bfc->getDescription());
    });

    //disable all bitfield stuff till we start tracking a bitfield
    setAllEnabled(bitFieldFrame, false);

    return bitFieldFrame;
}

void MainWindow::setAllEnabled(QWidget* parent, bool enabled){
    for (QObject* qo : parent->children()){
        if (QWidget* w = qobject_cast<QWidget*>(qo)){
            w->setEnabled(enabled);
        }
    }
}

QString MainWindow::getSortableString(uint64_t arg)
{
    //0-pad the base 10 offset by enough that there is no way a larger number will be sorted as "less" than a smaller number
    //due to leading digit problems
    //for example: 10 would be before 2, but 0010 will be after 0002
    return QString("%1").arg(QString::number(arg), 20, '0');
}

