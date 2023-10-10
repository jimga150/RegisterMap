#ifndef BITFIELDCONTROLLER_H
#define BITFIELDCONTROLLER_H

#include <QObject>

#include "bitfield.h"

class BitFieldController : public QObject
{
    Q_OBJECT
public:
    explicit BitFieldController(BitField* bf, QObject *parent = nullptr);
    ~BitFieldController();

    QString getName();
    QString getCodeName();
    bool getCodeNameGeneration();
    uint32_t getHighIdx();
    uint32_t getLowIdx();
    QString getDescription();
    QString getBitRangeAsString();

    inline static const std::string name_key = "name";
    inline static const std::string codename_key = "codename";
    inline static const std::string codenamegen_key = "autogen_codename";
    inline static const std::string desc_key = "description";
    inline static const std::string high_idx_key = "high_idx";
    inline static const std::string low_idx_key = "low_idx";

signals:

    void nameChanged(const QString& new_name);
    void codeNameChanged(const QString& new_name);
    void codeNameGenerationChanged(bool gen_codename);
    void rangeChanged(uint32_t low_idx, uint32_t high_idx);
    void rangeStrChanged(const QString& new_range_str);
    void descriptionChanged(const QString& new_desc);

    void changeMade();

public slots:

    void setName(const QString& new_name);
    void setCodeName(const QString& new_name);
    void setCodeNameGeneration(bool gen_codename);
    void setRange(uint32_t low_idx, uint32_t high_idx);
    void setDescription(const QString& new_desc);

private:

    bool gen_codename = true;

    BitField* bf;

};

#endif // BITFIELDCONTROLLER_H
