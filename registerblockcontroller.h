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

    addr_t getSize();

    int getCurrRegIdx();
    int getNumRegs();

    QString getCurrRegName();
    QString getCurrRegCodeName();
    bool getCurrRegCodeNameGeneration();
    addr_t getCurrRegOffset();
    QString getCurrRegOffsetAsString();

    QString getRegName(int reg_idx);
    QString getRegCodeName(int reg_idx);
    bool getRegCodeNameGeneration(int reg_idx);
    addr_t getRegOffset(int reg_idx);
    QString getRegOffsetAsString(int reg_idx);

signals:

    void nameChanged(const QString& new_name);
    void codeNameChanged(const QString& new_name);
    void codeNameGenerationChanged(bool gen_code_name);

    void sizeChanged(addr_t new_size);

    void currRegIdxChanged(int new_idx);

    void regCreated(const QString& name, addr_t offset, const QString& description);

    void regNameChanged(const QString& new_name);
    void regCodeNameChanged(const QString& new_name);
    void regCodeNameGenerationChanged(bool gen_codename);
    void regOffsetChanged(addr_t new_offset);

public slots:

    void setName(const QString& new_name);
    void setCodeName(const QString& new_name);
    void setCodeNameGeneration(bool gen_code_name);

    void setSize(const addr_t new_size);

    void setCurrRegIdx(int new_idx);
    void sortRegsByOffset();

    void makeNewReg();

    void setRegName(const QString& new_name);
    void setRegCodeName(const QString& new_name);
    void setRegCodeNameGeneration(bool gen_codename);
    void setRegOffset(addr_t new_offset);

private:

    bool gen_codename = true;

    std::vector<bool> gen_reg_codenames;

    int current_reg_idx = 0;

    RegisterBlock rb;

};

#endif // REGISTERBLOCKCONTROLLER_H
