INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/DS_Client.h \
    $$PWD/include/DS_Common.h \
    $$PWD/include/DS_Events.h \
    $$PWD/include/DS_Protocol.h \
    $$PWD/include/LibDS.h

SOURCES += \
    $$PWD/src/Protocols/FRC_2014.c \
    $$PWD/src/Protocols/FRC_2015.c \
    $$PWD/src/DS_Client.c \
    $$PWD/src/DS_Config.c \
    $$PWD/src/DS_Events.c \
    $$PWD/src/DS_Handler.c \
    $$PWD/src/DS_Joysticks.c \
    $$PWD/src/DS_NetConsole.c \
    $$PWD/src/DS_Protocol.c \
    $$PWD/src/DS_Sockets.c

HEADERS += \
    $$PWD/src/DS_Private.h

