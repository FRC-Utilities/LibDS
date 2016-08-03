CONFIG += console

CONFIG -= qt
CONFIG -= app_bundle

DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

TARGET = console-ds

LIBS += -lncurses
SOURCES += $$PWD/ConsoleDS.c

include ($$PWD/../LibDS.pri)
