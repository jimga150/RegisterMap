#include "bitfieldcontroller.h"

BitFieldController::BitFieldController(BitField bf, QObject *parent)
    : QObject{parent}
{
    this->bf = bf;
    connect(this, &BitFieldController::rangeChanged, this, [=](){
        emit this->rangeStrChanged(this->getBitRangeAsString());
    });
}

BitFieldController::~BitFieldController()
{

}

QString BitFieldController::getName()
{
    return this->bf.name.c_str();
}

QString BitFieldController::getCodeName()
{
    return this->bf.codename.c_str();
}

bool BitFieldController::getCodeNameGeneration()
{
    return this->gen_codename;
}

uint32_t BitFieldController::getHighIdx()
{
    return this->bf.high_index;
}

uint32_t BitFieldController::getLowIdx()
{
    return this->bf.low_index;
}

QString BitFieldController::getDescription()
{
    return this->bf.description.c_str();
}

QString BitFieldController::getBitRangeAsString()
{
    uint32_t low = this->getLowIdx();
    uint32_t high = this->getHighIdx();
    if (low == high) return QString::number(low);
    return QString::number(high) + ":" + QString::number(low);
}

void BitFieldController::setName(const QString& new_name)
{
    if (!(new_name.compare(this->bf.name.c_str()))) return;

    this->bf.name = new_name.toStdString();
    emit this->nameChanged(new_name);
    emit this->changeMade();

    if (this->gen_codename){
        std::string codename = generate_code_name(this->bf.name);
        this->setCodeName(codename.c_str());
    }
}

void BitFieldController::setCodeName(const QString& new_name)
{
    if (!(new_name.compare(this->bf.codename.c_str()))) return;

    this->bf.codename = new_name.toStdString();
    emit this->codeNameChanged(new_name);
    emit this->changeMade();
}

void BitFieldController::setCodeNameGeneration(bool gen_codename)
{
    if (this->gen_codename == gen_codename) return;

    this->gen_codename = gen_codename;
    emit this->codeNameGenerationChanged(gen_codename);
    emit this->changeMade();

    if (gen_codename){
        std::string codename = generate_code_name(this->bf.name);
        this->setCodeName(codename.c_str());
    }

}

void BitFieldController::setRange(uint32_t low_idx, uint32_t high_idx)
{
    if (low_idx == this->bf.low_index && high_idx == this->bf.high_index) return;

    this->bf.low_index = low_idx;
    this->bf.high_index = high_idx;
    emit this->rangeChanged(low_idx, high_idx);
    emit this->changeMade();
}

void BitFieldController::setDescription(const QString& new_desc)
{
    if (!(new_desc.compare(this->bf.description.c_str()))) return;

    this->bf.description = new_desc.toStdString();
    emit this->descriptionChanged(new_desc);
    emit this->changeMade();
}
