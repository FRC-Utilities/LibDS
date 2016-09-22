QT += gui
QT += widgets
QT += network

CONFIG += c++11

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/DriverStation.h \
    $$PWD/EventLogger.h

SOURCES += \
    $$PWD/DriverStation.cpp \
    $$PWD/EventLogger.cpp
