#ifndef REGISTERBLOCKCONTROLLER_H
#define REGISTERBLOCKCONTROLLER_H

#include <QObject>

#include "registerblock.h"

class CodeNameValidator;

class RegisterBlockController : public QObject
{
    Q_OBJECT
public:
    explicit RegisterBlockController(QVector<RegisterBlockController*>* containing_list, QObject *parent = nullptr);

    QString getName();
    QString getCodeName();
    bool getCodeNameGeneration();

    uint32_t getSize();

    int getCurrRegIdx();
    int getNumRegs();

    QString getCurrRegName();
    QString getCurrRegCodeName();
    bool getCurrRegCodeNameGeneration();
    uint32_t getCurrRegOffset();

    QString getRegName(int reg_idx);
    QString getRegCodeName(int reg_idx);
    bool getRegCodeNameGeneration(int reg_idx);
    uint32_t getRegOffset(int reg_idx);

signals:

    void nameChanged(const QString& new_name);
    void codeNameChanged(const QString& new_name);
    void codeNameGenerationChanged(bool gen_code_name);

    void sizeChanged(uint32_t new_size);

    void currRegIdxChanged(int new_idx);

    void regCreated(const QString& name, uint32_t offset, const QString& description);

    void regNameChanged(const QString& new_name);
    void regCodeNameChanged(const QString& new_name);
    void regCodeNameGenerationChanged(bool gen_codename);
    void regOffsetChanged(uint32_t new_offset);

public slots:

    void setName(const QString& new_name);
    void setCodeName(const QString& new_name);
    void setCodeNameGeneration(bool gen_code_name);

    void setSize(const uint32_t new_size);

    void setCurrRegIdx(int new_idx);

    void makeNewReg();

    void setRegName(const QString& new_name);
    void setRegCodeName(const QString& new_name);
    void setRegCodeNameGeneration(bool gen_codename);
    void setRegOffset(uint32_t new_offset);

private:

    bool gen_codename = true;

    std::vector<bool> gen_reg_codenames;

    int current_reg_idx = 0;

    RegisterBlock rb;

};

#endif // REGISTERBLOCKCONTROLLER_H
