CONFIG += console

CONFIG -= qt
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

SOURCES += $$PWD/example.c

include ($$PWD/../LibDS.pri)
