QT += gui
QT += widgets
QT += network

CONFIG += c++11

include ($$PWD/../../LibDS.pri)

TEMPLATE = app
TARGET = SimpleDS

HEADERS += \
  $$PWD/src/Window.h \
  $$PWD/src/VirtualJoystick.h \
    src/DriverStation.h

SOURCES += \
  $$PWD/src/main.cpp \
  $$PWD/src/Window.cpp \
  $$PWD/src/VirtualJoystick.cpp \
  $$PWD/src/DriverStation.cpp

FORMS += \
  $$PWD/src/Window.ui

RESOURCES += \
  $$PWD/resources/resources.qrc
