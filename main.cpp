#include "mainwindow.h"

#include <QApplication>

//TODO: make controller layer to get and set register/block info and update fields as appropriate
//TODO: make object to own regster blocks and domains and stuff, MainWindow will own that

//TODO: fix crash when editing one of several register blocks
//TODO: validate that no codenames collide
//TODO: use codenames for object IDs

//TODO: add register bit length
//TODO: add register byte length (from bit length, not saved)
//TODO: add register longoform description
//TODO: add register bit fields (table?)
//TODO: add bit field bit range
//TODO: add bit field name
//TODO: add bit field access rights(?)
//TODO: add bit field default val
//TODO: add bit field reset domains(?)
//TODO: add bit field description
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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
