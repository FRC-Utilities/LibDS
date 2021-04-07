TEMPLATE = lib
TARGET = LibDS

CONFIG -= qt

include ($$PWD/LibDS.pri)

unix {
    include.path = /usr/local/include/libds
    include.files = $$PWD/include/*

    target.path = /usr/local/lib

    INSTALLS += target
    INSTALLS += include
}
