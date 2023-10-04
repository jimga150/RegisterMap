#include "registerblockcontroller.h"

RegisterBlockController::RegisterBlockController(QVector<RegisterBlockController*>* containing_list, QObject *parent)
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

QString RegisterBlockController::getCurrRegOffsetAsString()
{
    return this->getRegOffsetAsString(this->current_reg_idx);
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

QString RegisterBlockController::getRegOffsetAsString(int reg_idx)
{
    return "0x" + QString::number(this->getRegOffset(reg_idx), 16);
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
    QString new_name_fixed = generate_code_name(new_name.toStdString()).c_str();
    this->rb.code_name = new_name_fixed.toStdString();
    emit this->codeNameChanged(new_name_fixed);
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

void RegisterBlockController::sortRegsByOffset()
{
    uint32_t offset_in_focus = this->getCurrRegOffset();

    this->rb.sort_registers_by_offset();

    //cycle through all register indices to cause table to update
    //with the info from new registers in given index positions
    for (int i = 0; i < this->getNumRegs(); ++i){
        this->setCurrRegIdx(i);
    }

    //return to whatever register we were focused on when we started
    for (uint i = 0; i < this->rb.registers.size(); ++i){
        if (this->rb.registers[i].offset == offset_in_focus){
            this->setCurrRegIdx(i);
            break;
        }
    }
}

void RegisterBlockController::makeNewReg()
{

    int new_idx = this->rb.registers.size();

    Register reg;
    reg.name = "New Register " + std::to_string(new_idx);
    reg.offset = new_idx; //TODO: check for offset collisions
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
    QString new_name_fixed = generate_code_name(new_name.toStdString()).c_str();
    this->rb.registers[this->current_reg_idx].code_name = new_name_fixed.toStdString();
    emit this->regCodeNameChanged(new_name_fixed);
}

void RegisterBlockController::setRegCodeNameGeneration(bool gen_codename)
{
    this->gen_reg_codenames.at(this->current_reg_idx) = gen_codename;
    emit this->regCodeNameGenerationChanged(gen_codename);

    if (gen_codename){
        this->setRegCodeName(
            generate_code_name(this->rb.registers[this->current_reg_idx].name).c_str()
        );
    }
}

void RegisterBlockController::setRegOffset(uint32_t new_offset)
{
    this->rb.registers[this->current_reg_idx].offset = new_offset;
    emit this->regOffsetChanged(new_offset);
}
