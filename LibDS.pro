INCLUDEPATH += $$PWD/include

HEADERS += \
    include/DS_Client.h \
    include/DS_Config.h \
    include/DS_Events.h \
    include/DS_Joysticks.h \
    include/DS_Objects.h \
    include/DS_Protocols.h \
    include/DS_Types.h \
    include/LibDS.h \
    include/DS_Utils.h

SOURCES += \
    src/client.c \
    src/config.c \
    src/events.c \
    src/init.c \
    src/joysticks.c \
    src/protocols.c \
    src/socket.c \
    src/protocols/frc_2016.c \
    src/utils.c \
    src/protocols/frc_2015.c \
    src/protocols/frc_2014.c
