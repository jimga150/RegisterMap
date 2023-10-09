#ifndef REGISTERCONTROLLER_H
#define REGISTERCONTROLLER_H

#include <QObject>

#include "bitfieldcontroller.h"
#include "register.h"

class RegisterController : public QObject
{
    Q_OBJECT
public:
    explicit RegisterController(Register* r, QObject *parent = nullptr);

    QString getName();
    QString getCodeName();
    bool getCodeNameGeneration();
    addr_t getOffset();
    QString getOffsetAsString();
    uint32_t getBitLen();
    QString getDescription();

    size_t getCurrBitFieldIdx();
    size_t getNumBitFields();
    BitFieldController* getCurrBitFieldController();
    BitFieldController* getBitFieldControllerAt(size_t n);

    inline static const std::string name_key = "name";
    inline static const std::string codename_key = "codename";
    inline static const std::string codenamegen_key = "autogen_codename";
    inline static const std::string desc_key = "description";
    inline static const std::string offset_key = "offset";
    inline static const std::string bitlen_key = "bit_len";
    inline static const std::string bitfields_key = "bit_fields";

signals:

    void nameChanged(const QString& new_name);
    void codeNameChanged(const QString& new_name);
    void codeNameGenerationChanged(bool gen_codename);
    void offsetChanged(addr_t new_offset);
    void bitLenChanged(uint32_t new_bitlen);
    void descriptionChanged(const QString& new_desc);

    void bitFieldCreated(BitFieldController* bfc);

    void currBitFieldIdxChanged(int new_idx);

    void changeMade();

public slots:

    void setName(const QString& new_name);
    void setCodeName(const QString& new_name);
    void setCodeNameGeneration(bool gen_codename);
    void setOffset(addr_t new_offset);
    void setBitLen(uint32_t new_bitlen);
    void setDescription(const QString& new_desc);

    void makeNewBitField();

    void setBitFieldIdx(size_t new_idx);

private:

    bool gen_codename = true;

    Register* reg;

    //set to a nonzero number so that the first bitfield creation will properly trigger the 0-index update
    size_t curr_bitfield_idx = -1;
    std::vector<BitFieldController*> bit_field_controllers;

};

#endif // REGISTERCONTROLLER_H
