QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bitfield.cpp \
    common.cpp \
    domain.cpp \
    main.cpp \
    mainwindow.cpp \
    register.cpp \
    registerblock.cpp \
    registerblockcontroller.cpp

HEADERS += \
    bitfield.h \
    common.h \
    domain.h \
    mainwindow.h \
    register.h \
    registerblock.h \
    registerblockcontroller.h \
    toml11/toml.hpp \
    toml11/toml/color.hpp \
    toml11/toml/combinator.hpp \
    toml11/toml/comments.hpp \
    toml11/toml/datetime.hpp \
    toml11/toml/exception.hpp \
    toml11/toml/from.hpp \
    toml11/toml/get.hpp \
    toml11/toml/into.hpp \
    toml11/toml/lexer.hpp \
    toml11/toml/literal.hpp \
    toml11/toml/macros.hpp \
    toml11/toml/parser.hpp \
    toml11/toml/region.hpp \
    toml11/toml/result.hpp \
    toml11/toml/serializer.hpp \
    toml11/toml/source_location.hpp \
    toml11/toml/storage.hpp \
    toml11/toml/string.hpp \
    toml11/toml/traits.hpp \
    toml11/toml/types.hpp \
    toml11/toml/utility.hpp \
    toml11/toml/value.hpp \
    toml11/toml/version.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    aaaa.toml
