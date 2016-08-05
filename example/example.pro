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
    LIBS += -L$$PWD/lib/sdl/win32/ -lSDL
    INCLUDEPATH += $$PWD/lib/sdl/win32/include
}

macx* {
    LIBS += -L$$PWD/lib/sdl/macx/ -lSDL
    INCLUDEPATH += $$PWD/lib/sdl/macx/include

    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework Cocoa
    LIBS += -framework CoreAudio
    LIBS += -framework IOKit
    LIBS += -framework OpenGL
    LIBS += -framework CoreFoundation
    LIBS += -framework Carbon
    LIBS += -framework ForceFeedback
    LIBS += -framework CoreVideo

    QMAKE_LFLAGS += -F /System/Library/Frameworks/AudioToolbox.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/AudioUnit.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Cocoa.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreAudio.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/IOKit.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/OpenGL.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Carbon.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/ForceFeedback.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreVideo.framework/
}

linux {
    LIBS += -lSDL
    INCLUDEPATH += /usr/include/SDL
}

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


