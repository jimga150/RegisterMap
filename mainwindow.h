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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    test_result_enum checkRBOffsetCollisions();
    test_result_enum checkOffsetCollisions(RegisterBlockController* rbc);

    test_result_enum checkRBCodeNameCollisions();

    test_result_enum checkRBRegCodeNameCollisions();
    test_result_enum checkRegCodeNameCollisions(RegisterBlockController* rbc);

    QVector<RegisterBlockController*> reg_block_ctrls;

    int current_reg_row; //TODO: link this to register blocks or tab widgets and store one for each

public slots:

    void save();

private slots:
    void on_new_reg_block_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
