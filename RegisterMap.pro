QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bitfield.cpp \
    domain.cpp \
    main.cpp \
    mainwindow.cpp \
    register.cpp \
    registerblock.cpp \
    registermap.cpp \
    toml11/tests/check.cpp \
    toml11/tests/check_serialization.cpp \
    toml11/tests/check_toml_test.cpp \
    toml11/tests/test_comments.cpp \
    toml11/tests/test_datetime.cpp \
    toml11/tests/test_error_detection.cpp \
    toml11/tests/test_expect.cpp \
    toml11/tests/test_extended_conversions.cpp \
    toml11/tests/test_find.cpp \
    toml11/tests/test_find_or.cpp \
    toml11/tests/test_find_or_recursive.cpp \
    toml11/tests/test_format_error.cpp \
    toml11/tests/test_get.cpp \
    toml11/tests/test_get_or.cpp \
    toml11/tests/test_lex_boolean.cpp \
    toml11/tests/test_lex_datetime.cpp \
    toml11/tests/test_lex_floating.cpp \
    toml11/tests/test_lex_integer.cpp \
    toml11/tests/test_lex_key_comment.cpp \
    toml11/tests/test_lex_string.cpp \
    toml11/tests/test_literals.cpp \
    toml11/tests/test_multiple_translation_unit_1.cpp \
    toml11/tests/test_multiple_translation_unit_2.cpp \
    toml11/tests/test_parse_array.cpp \
    toml11/tests/test_parse_boolean.cpp \
    toml11/tests/test_parse_datetime.cpp \
    toml11/tests/test_parse_file.cpp \
    toml11/tests/test_parse_floating.cpp \
    toml11/tests/test_parse_inline_table.cpp \
    toml11/tests/test_parse_integer.cpp \
    toml11/tests/test_parse_key.cpp \
    toml11/tests/test_parse_string.cpp \
    toml11/tests/test_parse_table.cpp \
    toml11/tests/test_parse_table_key.cpp \
    toml11/tests/test_parse_unicode.cpp \
    toml11/tests/test_result.cpp \
    toml11/tests/test_serialize_file.cpp \
    toml11/tests/test_string.cpp \
    toml11/tests/test_traits.cpp \
    toml11/tests/test_utility.cpp \
    toml11/tests/test_value.cpp \
    toml11/tests/test_windows.cpp

HEADERS += \
    bitfield.h \
    common.h \
    domain.h \
    mainwindow.h \
    register.h \
    registerblock.h \
    registermap.h \
    toml11/tests/test_lex_aux.hpp \
    toml11/tests/test_parse_aux.hpp \
    toml11/tests/unit_test.hpp \
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
    toml11/CMakeLists.txt \
    toml11/LICENSE \
    toml11/README.md \
    toml11/appveyor.yml \
    toml11/cmake/toml11Config.cmake.in \
    toml11/tests/CMakeLists.txt

