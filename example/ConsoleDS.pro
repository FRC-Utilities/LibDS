CONFIG += console

CONFIG -= qt
CONFIG -= app_bundle

DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

TARGET = console-ds

win32* {
    INCLUDEPATH += $$PWD/pdcurses
    LIBS += -L$$PWD/pdcurses/ -lSDL
    LIBS += -L$$PWD/pdcurses/ -lpdcurses
}

else {
    LIBS += -lncurses
}

SOURCES += $$PWD/ConsoleDS.c

include ($$PWD/../LibDS.pri)
