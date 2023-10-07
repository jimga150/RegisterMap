#ifndef REGISTERBLOCKCONTROLLER_H
#define REGISTERBLOCKCONTROLLER_H

#include <QObject>

#include "registerblock.h"

class CodeNameValidator;

class RegisterBlockController : public QObject
{
    Q_OBJECT
public:
    explicit RegisterBlockController(QObject *parent = nullptr);

    QString getName();
    QString getCodeName();
    bool getCodeNameGeneration();
    QString getDescription();

    addr_t getSize();

    int getCurrRegIdx();
    int getCurrBitFieldIdx();
    int getNumRegs();

    int getCurrNumBitFields();
    int getNumBitFields(int reg_idx);

    QString getCurrRegName();
    QString getCurrRegCodeName();
    bool getCurrRegCodeNameGeneration();
    addr_t getCurrRegOffset();
    QString getCurrRegOffsetAsString();
    uint32_t getCurrRegBitLen();
    QString getCurrRegDescription();
    BitField& getCurrRegCurrBitField();
    BitField& getCurrRegBitField(int bitfield_idx);

    QString getRegName(int reg_idx);
    QString getRegCodeName(int reg_idx);
    bool getRegCodeNameGeneration(int reg_idx);
    addr_t getRegOffset(int reg_idx);
    QString getRegOffsetAsString(int reg_idx);
    uint32_t getRegBitLen(int reg_idx);
    QString getRegDescription(int reg_idx);

    BitField& getRegBitField(int reg_idx, int bitfield_idx);
    bool getRegBitFieldCodeNameGeneration(int reg_idx, int bitfield_idx);

    static QString getBitRangeAsString(uint32_t low_idx, uint32_t high_idx);

    inline static const std::string name_key = "name";
    inline static const std::string codename_key = "codename";
    inline static const std::string codenamegen_key = "autogen_codename";
    inline static const std::string desc_key = "description";
    inline static const std::string size_key = "size";
    inline static const std::string reg_key = "registers";

    inline static const std::string reg_name_key = "name";
    inline static const std::string reg_codename_key = "codename";
    inline static const std::string reg_codenamegen_key = "autogen_codename";
    inline static const std::string reg_desc_key = "description";
    inline static const std::string reg_offset_key = "offset";
    inline static const std::string reg_bitlen_key = "bit_len";
    inline static const std::string reg_bitfields_key = "bit_fields";

    inline static const std::string bitfield_name_key = "name";
    inline static const std::string bitfield_codename_key = "codename";
    inline static const std::string bitfield_codenamegen_key = "autogen_codename";
    inline static const std::string bitfield_desc_key = "description";
    inline static const std::string bitfield_high_idx_key = "high_idx";
    inline static const std::string bitfield_low_idx_key = "low_idx";

signals:

    void changeMade();

    void nameChanged(const QString& new_name);
    void codeNameChanged(const QString& new_name);
    void codeNameGenerationChanged(bool gen_code_name);
    void descriptionChanged(const QString& new_desc);

    void sizeChanged(addr_t new_size);

    void currRegIdxChanged(int new_idx);

    void regCreated(const QString& name, addr_t offset, const QString& description);

    void regNameChanged(const QString& new_name);
    void regCodeNameChanged(const QString& new_name);
    void regCodeNameGenerationChanged(bool gen_codename);
    void regOffsetChanged(addr_t new_offset);
    void regBitLenChanged(uint32_t new_bitlen);
    void regDescriptionChanged(const QString& new_desc);

    void bitFieldCreated(const QString& name, uint32_t low_idx, uint32_t high_idx, const QString& description);

    void currBitFieldIdxChanged(int new_idx);

    void bitFieldNameChanged(const QString& new_name);
    void bitFieldCodeNameChanged(const QString& new_name);
    void bitFieldCodeNameGenerationChanged(bool gen_codename);
    void bitFieldRangeChanged(uint32_t low_idx, uint32_t high_idx);
    void bitFieldDescriptionChanged(const QString& new_desc);

public slots:

    void setName(const QString& new_name);
    void setCodeName(const QString& new_name);
    void setCodeNameGeneration(bool gen_code_name);
    void setDescription(const QString& new_desc);

    void setSize(const addr_t new_size);

    void setCurrRegIdx(int new_idx);
    void sortRegsByOffset();

    void makeNewReg();

    void setRegName(const QString& new_name);
    void setRegCodeName(const QString& new_name);
    void setRegCodeNameGeneration(bool gen_codename);
    void setRegOffset(addr_t new_offset);
    void setRegBitLen(uint32_t new_bitlen);
    void setRegDescription(const QString& new_desc);

    void makeNewBitField();

    void setCurrBitFieldIdx(int new_idx);

    void setBitFieldName(const QString& new_name);
    void setBitFieldCodeName(const QString& new_name);
    void setBitFieldCodeNameGeneration(bool gen_codename);
    void setBitFieldRange(uint32_t low_idx, uint32_t high_idx);
    void setBitFieldDescription(const QString& new_desc);

private:

    bool gen_codename = true;

    std::vector<bool> gen_reg_codenames;

    std::vector<int> reg_bitfield_idxs;
    std::vector<std::vector<bool>> gen_bitfield_codenames;

    int current_reg_idx = 0;

    RegisterBlock rb;

};

#endif // REGISTERBLOCKCONTROLLER_H
