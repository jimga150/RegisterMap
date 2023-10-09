#ifndef REGISTERBLOCKCONTROLLER_H
#define REGISTERBLOCKCONTROLLER_H

#include <QObject>

#include "registerblock.h"
#include "registercontroller.h"

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

    size_t getCurrRegIdx();
    size_t getNumRegs();
    RegisterController* getCurrRegController();
    RegisterController* getRegControllerAt(size_t n);

    inline static const std::string name_key = "name";
    inline static const std::string codename_key = "codename";
    inline static const std::string codenamegen_key = "autogen_codename";
    inline static const std::string desc_key = "description";
    inline static const std::string size_key = "size";
    inline static const std::string reg_key = "registers";

signals:

    void changeMade();

    void nameChanged(const QString& new_name);
    void codeNameChanged(const QString& new_name);
    void codeNameGenerationChanged(bool gen_code_name);
    void descriptionChanged(const QString& new_desc);

    void sizeChanged(addr_t new_size);

    void currRegIdxChanged(int new_idx);
    void currBitFieldIdxChanged(int new_idx);

    void regCreated(RegisterController* rc);

    void regIdxsReassigned();

public slots:

    void setName(const QString& new_name);
    void setCodeName(const QString& new_name);
    void setCodeNameGeneration(bool gen_code_name);
    void setDescription(const QString& new_desc);

    void setSize(const addr_t new_size);

    void setCurrRegIdx(int new_idx);
    void sortRegsByOffset();

    void makeNewReg();

private:

    bool gen_codename = true;

    RegisterBlock rb;

    size_t current_reg_idx;
    std::vector<RegisterController*> reg_controllers;

};

#endif // REGISTERBLOCKCONTROLLER_H
