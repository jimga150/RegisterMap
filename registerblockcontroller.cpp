#include "registerblockcontroller.h"

RegisterBlockController::RegisterBlockController(QObject *parent)
    : QObject{parent}
{
    //if the current register index changes, notify any listeners about the current info changes
    connect(this, &RegisterBlockController::currRegIdxChanged, this, [=](int new_idx){
        Q_UNUSED(new_idx)
        emit this->regNameChanged(this->getCurrRegName());
        emit this->regCodeNameChanged(this->getCurrRegCodeName());
        emit this->regCodeNameGenerationChanged(this->getCurrRegCodeNameGeneration());
        emit this->regOffsetChanged(this->getCurrRegOffset());
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

uint32_t RegisterBlockController::getSize()
{
    return this->rb.size;
}

int RegisterBlockController::getCurrRegIdx()
{
    return this->current_reg_idx;
}

int RegisterBlockController::getNumRegs()
{
    return this->rb.registers.size();
}

QString RegisterBlockController::getCurrRegName()
{
    return this->rb.registers[this->current_reg_idx].name.c_str();
}

QString RegisterBlockController::getCurrRegCodeName()
{
    return this->rb.registers[this->current_reg_idx].code_name.c_str();
}

bool RegisterBlockController::getCurrRegCodeNameGeneration()
{
    return this->gen_reg_codenames[this->current_reg_idx];
}

uint32_t RegisterBlockController::getCurrRegOffset()
{
    return this->rb.registers[this->current_reg_idx].offset;
}

QString RegisterBlockController::getRegName(int reg_idx)
{
    return this->rb.registers[reg_idx].name.c_str();
}

QString RegisterBlockController::getRegCodeName(int reg_idx)
{
    return this->rb.registers[reg_idx].code_name.c_str();
}

bool RegisterBlockController::getRegCodeNameGeneration(int reg_idx)
{
    return this->gen_reg_codenames[reg_idx];
}

uint32_t RegisterBlockController::getRegOffset(int reg_idx)
{
    return this->rb.registers[reg_idx].offset;
}

void RegisterBlockController::setName(const QString& new_name)
{
    this->rb.name = new_name.toStdString();
    emit this->nameChanged(new_name);

    if (gen_codename){
        this->rb.code_name = generate_code_name(this->rb.name);
        emit this->codeNameChanged(this->rb.code_name.c_str());
    }
}

void RegisterBlockController::setCodeName(const QString& new_name)
{
    this->rb.code_name = new_name.toStdString();
    emit this->codeNameChanged(new_name);
}

void RegisterBlockController::setCodeNameGeneration(bool gen_code_name)
{
    this->gen_codename = gen_code_name;
    emit this->codeNameGenerationChanged(gen_code_name);

    if (gen_codename){
        this->rb.code_name = generate_code_name(this->rb.name);
        emit this->codeNameChanged(this->rb.code_name.c_str());
    }
}

void RegisterBlockController::setSize(const uint32_t new_size)
{
    this->rb.size = new_size;
    emit this->sizeChanged(new_size);
}

void RegisterBlockController::setCurrRegIdx(int new_idx)
{
    this->current_reg_idx = new_idx;
    emit this->currRegIdxChanged(new_idx);
}

void RegisterBlockController::makeNewReg()
{
    Register reg;
    reg.name = "New Register";
    reg.offset = this->rb.registers.size();
    reg.code_name = generate_code_name(reg.name);
    reg.bit_len = 8; //TODO: set default for this in a menu? default per-block?
    reg.description = "Reserved";

    this->rb.registers.push_back(reg);
    this->gen_reg_codenames.push_back(true);

    emit this->regCreated(reg.name.c_str(), reg.offset, reg.description.c_str());
}

void RegisterBlockController::setRegName(const QString& new_name)
{
    this->rb.registers[this->current_reg_idx].name = new_name.toStdString();
    emit this->regNameChanged(new_name);

    if (this->gen_reg_codenames[this->current_reg_idx]){
        this->setRegCodeName(generate_code_name(this->rb.registers[this->current_reg_idx].name).c_str());
    }
}

void RegisterBlockController::setRegCodeName(const QString& new_name)
{
    this->rb.registers[this->current_reg_idx].name = new_name.toStdString();
    emit this->regCodeNameChanged(new_name);
}

void RegisterBlockController::setRegCodeNameGeneration(bool gen_codename)
{
    this->gen_codename = gen_codename;
    emit this->codeNameGenerationChanged(gen_codename);
}

void RegisterBlockController::setRegOffset(uint32_t new_offset)
{
    this->rb.registers[this->current_reg_idx].offset = new_offset;
    emit this->regOffsetChanged(new_offset);
}
