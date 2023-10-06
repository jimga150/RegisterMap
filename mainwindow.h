#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QtWidgets/qtablewidget.h"
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

    QWidget* makeNewRegBlockTab();

    QFrame* makeNewRegFrame(RegisterBlockController* rbc, QTableWidget* regTable);

    QFrame* makeNewBitFieldFrame(RegisterBlockController* rbc, QTableWidget* bitFieldTable);

    void setAllEnabled(QWidget* parent, bool enabled);

    test_result_enum checkRBOffsetCollisions();
    test_result_enum checkOffsetCollisions(RegisterBlockController* rbc);

    test_result_enum checkRBCodeNameCollisions();

    test_result_enum checkRBRegCodeNameCollisions();
    test_result_enum checkRegCodeNameCollisions(RegisterBlockController* rbc);

    void print_toml_table(toml_value_t table, int tab_level);

    void loadRegisterBlock(toml_value_t reg_block_table, std::string table_key);

    void loadRegister(toml_value_t reg_table, std::string table_key, RegisterBlockController* rbc);

    void loadBitField(toml_value_t bitfield_table, std::string table_key, RegisterBlockController* rbc);



    QVector<RegisterBlockController*> reg_block_ctrls;

    int current_reg_row; //TODO: link this to register blocks or tab widgets and store one for each

    void (*makeNewWindow)(QString load_filename) = nullptr;

    inline static const std::string vmaj_key = "version_major";
    inline static const std::string vmin_key = "version_minor";

    inline static const std::string reg_block_prefix = "rb_";

public slots:

    void save();

    void load();

    void load_file(QString load_file);

    void changeMade();

private slots:
    void on_new_reg_block_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
