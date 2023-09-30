#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QtWidgets/qtablewidget.h"
#include "registermap.h"
#include "toml11/toml.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    RegisterMap reg_map;

    //associates tabs with registerblocks
    std::map<QWidget*, RegisterBlock*> reg_blocks;

    int current_reg_row; //TODO: link this to register blocks or tab widgets and store one for each

    QRegularExpressionValidator* codename_validator;

public slots:
    void gen_regblk_code_name(const QString &new_text);

    void set_regblk_codename_generation(int custom_codename_checked);

    void make_new_reg();

    void set_reg_block_name(const QString &new_name);

    void set_reg_block_codename(const QString &new_name);

    void set_reg_block_size(int new_size);

    void set_current_reg(QTableWidgetItem* item);

    void set_reg_name(const QString& new_name);

    void set_reg_codename(const QString& new_codename);

    void set_reg_codename_gen(int custom_codename_checked);

    void set_reg_offset(int new_offset);

    void save();

private slots:
    void on_new_reg_block_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
