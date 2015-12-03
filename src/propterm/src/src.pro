include(../common.pri)

CONFIG += staticlib

TARGET = propterm
TEMPLATE = lib
DESTDIR = ../lib/

SOURCES += \
    propterm.cpp \
    console.cpp

HEADERS += \
    propterm.h \
    console.h

FORMS += \
    propterm.ui \
