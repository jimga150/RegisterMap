#include "registercontroller.h"

RegisterController::RegisterController(Register r, QObject *parent)
    : QObject{parent}
{
    this->reg = r;
}

RegisterController::~RegisterController()
{

}

QString RegisterController::getName()
{
    return this->reg.name.c_str();
}

QString RegisterController::getCodeName()
{
    return this->reg.code_name.c_str();
}

bool RegisterController::getCodeNameGeneration()
{
    return this->gen_codename;
}

addr_t RegisterController::getOffset()
{
    return this->reg.offset;
}

QString RegisterController::getOffsetAsString()
{
    return "0x" + QString::number(this->reg.offset, 16);
}

uint32_t RegisterController::getBitLen()
{
    return this->reg.bit_len;
}

uint32_t RegisterController::getByteLen()
{
    return this->reg.getByteLen();
}

QString RegisterController::getDescription()
{
    return this->reg.description.c_str();
}

size_t RegisterController::getCurrBitFieldIdx()
{
    return this->curr_bitfield_idx;
}

size_t RegisterController::getNumBitFields()
{
    return this->bit_field_controllers.size();
}

BitFieldController* RegisterController::getCurrBitFieldController()
{
    return this->getBitFieldControllerAt(this->getCurrBitFieldIdx());
}

BitFieldController* RegisterController::getBitFieldControllerAt(size_t n)
{
    return this->bit_field_controllers.at(n);
}

void RegisterController::setName(const QString& new_name)
{
    if (!(new_name.compare(this->reg.name.c_str()))) return;

    this->reg.name = new_name.toStdString();
    emit this->nameChanged(new_name);
    emit this->changeMade();

    if (this->gen_codename){
        this->setCodeName(generate_code_name(this->reg.name).c_str());
    }
}

void RegisterController::setCodeName(const QString& new_name)
{
    if (!(new_name.compare(this->reg.code_name.c_str()))) return;

    QString new_name_fixed = generate_code_name(new_name.toStdString()).c_str();
    this->reg.code_name = new_name_fixed.toStdString();
    emit this->codeNameChanged(new_name_fixed);
    emit this->changeMade();
}

void RegisterController::setCodeNameGeneration(bool gen_codename)
{
    if (gen_codename == this->gen_codename) return;

    this->gen_codename = gen_codename;
    emit this->codeNameGenerationChanged(gen_codename);
    emit this->changeMade();

    if (gen_codename){
        this->setCodeName(generate_code_name(this->reg.name).c_str());
    }
}

void RegisterController::setOffset(addr_t new_offset)
{
    if (new_offset == this->reg.offset) return;

    this->reg.offset = new_offset;

    emit this->offsetChanged(new_offset);
    emit this->changeMade();
}

void RegisterController::setBitLen(uint32_t new_bitlen)
{
    if (new_bitlen == this->reg.bit_len) return;

    //TODO: there will likely be some heavy ramifications of this if it ends up being too small for the existing bitfields.
    //Also, will this be compatible with the interface(s)???
    this->reg.bit_len = new_bitlen;

    emit this->bitLenChanged(new_bitlen);
    emit this->changeMade();
}

void RegisterController::setDescription(const QString& new_desc)
{
    if (!(new_desc.compare(this->reg.description.c_str()))) return;

    this->reg.description = new_desc.toStdString();
    emit this->descriptionChanged(new_desc);
}

void RegisterController::makeNewBitField()
{
    BitField b;
    b.name = "BitField " + std::to_string(this->getNumBitFields());
    b.codename = generate_code_name(b.name);
    b.low_index = 0;
    b.high_index = 0;

    BitFieldController* bfc = new BitFieldController(b, this);
    connect(bfc, &BitFieldController::changeMade, this, &RegisterController::changeMade);

    this->bit_field_controllers.push_back(bfc);

    emit this->bitFieldCreated(bfc);
    emit this->changeMade();
}

void RegisterController::deleteBitField(size_t idx)
{
    bool change_idx = this->getNumBitFields() > 1;
    size_t new_idx = idx > 0 ? idx - 1 : idx;

    BitFieldController* bfc_todelete = this->getBitFieldControllerAt(idx);

    this->bit_field_controllers.erase(this->bit_field_controllers.begin() + idx);

    bfc_todelete->deleteLater();
    //BitFieldController will emit destroyed() signal, which is connected to the UI elements

    emit this->changeMade();

    if (change_idx) this->setBitFieldIdx(new_idx);
}

void RegisterController::setBitFieldIdx(size_t new_idx)
{
    if (new_idx == this->curr_bitfield_idx) return;

    this->curr_bitfield_idx = new_idx;
    emit this->currBitFieldIdxChanged(new_idx);
}

void RegisterController::sortBitFieldsByRange()
{
    if (this->getNumBitFields() == 0) return;

    QString range_in_focus = this->getCurrBitFieldController()->getBitRangeAsString();

    std::vector<BitFieldController*> sorted_bit_field_controllers;
    sorted_bit_field_controllers.reserve(this->bit_field_controllers.size());

    //sort bit field controller list (these point to registers owned by the register block,
    for (uint i = 0; i < this->bit_field_controllers.size(); ++i){
        uint j;
        for (j = 0; j < sorted_bit_field_controllers.size(); ++j){
            if (sorted_bit_field_controllers[j]->getLowIdx() > this->bit_field_controllers[i]->getLowIdx()){
                break;
            }
        }
        sorted_bit_field_controllers.insert(sorted_bit_field_controllers.begin() + j, this->bit_field_controllers[i]);
    }
    this->bit_field_controllers.swap(sorted_bit_field_controllers);

    //cycle through all bit field indices to cause table to update
    //with the info from new registers in given index positions
    for (uint i = 0; i < this->getNumBitFields(); ++i){
        emit this->currBitFieldIdxChanged(i);
    }

    emit this->bitFieldIdxsReassigned();

    //return to whatever bit field we were focused on when we started
    for (uint i = 0; i < this->getNumBitFields(); ++i){
        if (!(this->bit_field_controllers[i]->getBitRangeAsString().compare(range_in_focus))){
            this->setBitFieldIdx(i);
            break;
        }
    }
}
