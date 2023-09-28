#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    std::map<QWidget*, RegisterBlock*> reg_blocks;

public slots:
    void gen_code_name(const QString &new_text);

    void set_codename_generation(int custom_codename_checked);

    void set_reg_block_name(const QString &new_name);

    void save();

private slots:
    void on_new_reg_block_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
