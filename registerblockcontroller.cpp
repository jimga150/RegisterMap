#include "registerblockcontroller.h"

RegisterBlockController::RegisterBlockController(QObject *parent)
    : QObject{parent}
{
    connect(this, &RegisterBlockController::currRegIdxChanged, this, [=](int new_reg_idx){
        Q_UNUSED(new_reg_idx);
        RegisterController* rc = this->getCurrRegController();
        if (rc->getNumBitFields() > 0) emit this->currBitFieldIdxChanged(rc->getCurrBitFieldIdx());
    });
}

QString RegisterBlockController::getName()
{
    return this->rb.name.c_str();
}

QString RegisterBlockController::getCodeName()
{
    return this->rb.code_name.c_str();
}

bool RegisterBlockController::getCodeNameGeneration()
{
    return this->gen_codename;
}

QString RegisterBlockController::getDescription()
{
    return this->rb.description.c_str();
}

addr_t RegisterBlockController::getSize()
{
    return this->rb.size;
}

size_t RegisterBlockController::getCurrRegIdx()
{
    return this->current_reg_idx;
}

size_t RegisterBlockController::getNumRegs()
{
    return this->reg_controllers.size();
}

RegisterController* RegisterBlockController::getCurrRegController()
{
    return this->getRegControllerAt(this->getCurrRegIdx());
}

RegisterController* RegisterBlockController::getRegControllerAt(size_t n)
{
    return this->reg_controllers.at(n);
}

void RegisterBlockController::setName(const QString& new_name)
{
    if (!(new_name.compare(this->rb.name.c_str()))) return;

    this->rb.name = new_name.toStdString();
    emit this->nameChanged(new_name);
    emit this->changeMade();

    if (gen_codename){
        this->rb.code_name = generate_code_name(this->rb.name);
        emit this->codeNameChanged(this->rb.code_name.c_str());
    }
}

void RegisterBlockController::setCodeName(const QString& new_name)
{
    if (!(new_name.compare(this->rb.code_name.c_str()))) return;

    QString new_name_fixed = generate_code_name(new_name.toStdString()).c_str();
    this->rb.code_name = new_name_fixed.toStdString();
    emit this->codeNameChanged(new_name_fixed);
    emit this->changeMade();
}

void RegisterBlockController::setCodeNameGeneration(bool gen_code_name)
{
    if (this->gen_codename == gen_code_name) return;

    this->gen_codename = gen_code_name;
    emit this->codeNameGenerationChanged(gen_code_name);
    emit this->changeMade();

    if (gen_codename){
        this->rb.code_name = generate_code_name(this->rb.name);
        emit this->codeNameChanged(this->rb.code_name.c_str());
    }
}

void RegisterBlockController::setDescription(const QString& new_desc)
{
    if (!(new_desc.compare(this->rb.description.c_str()))) return;

    this->rb.description = new_desc.toStdString();
    emit this->descriptionChanged(new_desc);
    emit this->changeMade();
}

void RegisterBlockController::setSize(const addr_t new_size)
{
    if (this->rb.size == new_size) return;

    this->rb.size = new_size;
    emit this->sizeChanged(new_size);
    emit this->changeMade();
}

void RegisterBlockController::setCurrRegIdx(int new_idx)
{
    this->current_reg_idx = new_idx;
    emit this->currRegIdxChanged(new_idx);

    RegisterController* rc = this->getCurrRegController();

    if (rc->getNumBitFields() > 0){
        emit rc->currBitFieldIdxChanged(rc->getCurrBitFieldIdx());
    }
}

void RegisterBlockController::sortRegsByOffset()
{
    if (this->getNumRegs() == 0) return;

    addr_t offset_in_focus = this->getCurrRegController()->getOffset();

    std::vector<RegisterController*> sorted_reg_controllers;
    sorted_reg_controllers.reserve(this->reg_controllers.size());

    //sort register controller list (these point to registers owned by the register block,
    //so that list doesnt need to be sorted)
    for (uint i = 0; i < this->reg_controllers.size(); ++i){
        uint j;
        for (j = 0; j < sorted_reg_controllers.size(); ++j){
            if (sorted_reg_controllers[j]->getOffset() > this->reg_controllers[i]->getOffset()){
                break;
            }
        }
        sorted_reg_controllers.insert(sorted_reg_controllers.begin() + j, this->reg_controllers[i]);
    }
    this->reg_controllers.swap(sorted_reg_controllers);

    //cycle through all register indices to cause table to update
    //with the info from new registers in given index positions
    for (uint i = 0; i < this->getNumRegs(); ++i){
        this->setCurrRegIdx(i);
    }

    emit this->regIdxsReassigned();

    //return to whatever register we were focused on when we started
    for (uint i = 0; i < this->getNumRegs(); ++i){
        if (this->reg_controllers[i]->getOffset() == offset_in_focus){
            this->setCurrRegIdx(i);
            break;
        }
    }
}

void RegisterBlockController::makeNewReg()
{

    int new_idx = this->getNumRegs();

    Register* reg_addr = new Register;

    reg_addr->name = "New Register " + std::to_string(new_idx);
    reg_addr->offset = new_idx; //TODO: check for offset collisions
    reg_addr->code_name = generate_code_name(reg_addr->name);
    reg_addr->bit_len = 8; //TODO: set default for this in a menu? default per-block?
    reg_addr->description = "Reserved";

    //we take ownership of Register pointer
    RegisterController* rc = new RegisterController(reg_addr, this);

    connect(rc, &RegisterController::changeMade, this, &RegisterBlockController::changeMade);
    connect(rc, &RegisterController::currBitFieldIdxChanged, this, &RegisterBlockController::currBitFieldIdxChanged);

    this->reg_controllers.push_back(rc);

    emit this->regCreated(rc);
    emit this->changeMade();
}
