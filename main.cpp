#include "mainwindow.h"

#include <QApplication>

//TODO: make bitfields and registers not heap pointers in controllers?
//TODO: add register block deletion

//TODO: add bit field access rights(?)
//TODO: add bit field default val
//TODO: add bit field reset domains(?)
//TODO: add bit field enum option
//TODO: add bit field self-clearing option
//TODO: add register default value (derived, not saved)
//TODO: add register reset domains (derived, not saved)

//TODO: add register groups
//TODO: add register group name
//TODO: add register group codename
//TODO: add register group size
//TODO: add register group offset
//TODO: add register group last addr (derived, not saved)

//TODO: add PDF output
//TODO: add word doc output
//TODO: add C header output
//TODO: add verilog header output
//TODO: add verilog implementation output
//TODO: add VHDL package output
//TODO: add VHDL impl output

//TODO: make command line option to just export a toml file as one or more of the other outputs instead of running the whole GUI

void makeNewWindow(QString load_filename);
void makeNewWindow_noload();

static std::vector<MainWindow*> windows;

void makeNewWindow(QString load_filename){
    makeNewWindow_noload();
    windows.at(windows.size()-1)->loadFile(load_filename);
}

void makeNewWindow_noload(){
    MainWindow* w = new MainWindow(makeNewWindow);
    windows.push_back(w);
    w->show();

    if (windows.size() > 1){
        //shift down and to the right by 20 pixels, just to make the previous window visible
        MainWindow* last_window = windows.at(windows.size()-2);
        w->move(last_window->pos() + QPoint(20, 20));
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    makeNewWindow_noload();

    int retval = a.exec();

    for (MainWindow* w : windows){
        delete w;
    }

    return retval;
}
