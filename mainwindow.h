#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QTableWidget>
#include "registerblockcontroller.h"
#include "toml11/toml.hpp"

//override unordered_map to map to imply order
using toml_value_t = toml::basic_value<TOML11_DEFAULT_COMMENT_STRATEGY, std::map, std::vector>;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum test_result_enum{
    OK,
    ERROR,

    num_test_results
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(void (*makeNewWindow)(QString load_filename), QWidget *parent = nullptr);
    ~MainWindow();

    bool openSaveFile(QFile& file);

    QWidget* makeNewRegBlockTab();

    QFrame* makeNewRegFrame(RegisterBlockController* rbc);

    QFrame* makeNewBitFieldFrame(RegisterBlockController* rbc);

    void setAllEnabled(QWidget* parent, bool enabled);

    QString getSortableString(uint64_t arg);

    test_result_enum checkAllRegRangeCollisions();
    test_result_enum checkRBRegRangeCollisions(RegisterBlockController* rbc);
    test_result_enum checkRegBitLen(RegisterController* rc, uint32_t rbc_bitlen);
    test_result_enum checkRegRangeCollisions(RegisterController* rc, uint32_t rbc_bitlen);

    test_result_enum checkAllOffsetCollisions();
    test_result_enum checkOffsetCollisions(RegisterBlockController* rbc);

    test_result_enum checkRBCodeNameCollisions();

    test_result_enum checkAllRegCodeNameCollisions();
    test_result_enum checkRegCodeNameCollisions(RegisterBlockController* rbc);

    void printTomlTable(toml_value_t table, int tab_level);

    void loadRegisterBlock(toml_value_t reg_block_table, std::string table_key);

    void loadRegister(toml_value_t reg_table, std::string table_key, RegisterBlockController* rbc);

    void loadBitField(toml_value_t bitfield_table, std::string table_key, RegisterController* rc);



    QVector<RegisterBlockController*> reg_block_ctrls;

    std::vector<QMetaObject::Connection> reg_ui_connections;
    std::vector<QMetaObject::Connection> bitfield_ui_connections;

    int current_reg_row; //TODO: link this to register blocks or tab widgets and store one for each

    void (*makeNewWindow)(QString load_filename) = nullptr;

    inline static const std::string vmaj_key = "version_major";
    inline static const std::string vmin_key = "version_minor";

    inline static const std::string reg_block_prefix = "rb_";

public slots:

    void save();

    void saveTo(QFile* save_file, bool is_validated = false);

    void load();

    void loadFile(QString load_file);

    void changeMade();

private slots:
    void on_new_reg_block_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
