include("../propterm.pri")

CONFIG -= app_bundle debug_and_release
CONFIG += static

TARGET = propterm
TEMPLATE = lib

SOURCES += \
    propterm.cpp \
    console.cpp

HEADERS += \
    propterm.h \
    console.h

FORMS += \
    propterm.ui \

