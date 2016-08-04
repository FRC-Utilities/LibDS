#-------------------------------------------------------------------------------
# Remove Qt dependency
#-------------------------------------------------------------------------------

CONFIG += console

CONFIG -= qt
CONFIG -= app_bundle

DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

#-------------------------------------------------------------------------------
# Deploy options
#-------------------------------------------------------------------------------

TARGET = console-ds

#-------------------------------------------------------------------------------
# Link with ncurses/pdcurses
#-------------------------------------------------------------------------------

win32* {
    LIBS += -L$$PWD/lib/pdcurses/win32/ -lpdcurses
    INCLUDEPATH += $$PWD/lib/pdcurses/include
} else {
    LIBS += -lncurses
}

#-------------------------------------------------------------------------------
# Link with SDL
#-------------------------------------------------------------------------------

win32* {
   INCLUDEPATH += $$PWD/lib/sdl/include
   LIBS += -L$$PWD/lib/sdl/win32/ -lSDL
}
macx* {
   INCLUDEPATH += $$PWD/lib/sdl/include
   LIBS += -L$$PWD/lib/sdl/macx/ -lSDL
}
#linux {
#    INCLUDEPATH += /usr/include/SDL
#    LIBS += -lSDL
#}

#-------------------------------------------------------------------------------
# Include LibDS
#-------------------------------------------------------------------------------

include ($$PWD/../LibDS.pri)

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

HEADERS += \
    $$PWD/src/interface.h \
    $$PWD/src/joystick.h

SOURCES += \
    $$PWD/src/interface.c \
    $$PWD/src/joystick.c \
    $$PWD/src/main.c


