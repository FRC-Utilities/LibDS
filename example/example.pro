CONFIG += console

CONFIG -= qt
CONFIG -= app_bundle
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

SOURCES += $$PWD/example.c

include ($$PWD/../LibDS.pri)
