#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QtWidgets/qtablewidget.h"
#include "registerblockcontroller.h"
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

    QVector<RegisterBlockController*> reg_block_ctrls;

    int current_reg_row; //TODO: link this to register blocks or tab widgets and store one for each

    QRegularExpressionValidator* codename_validator;

public slots:

    void save();

private slots:
    void on_new_reg_block_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
