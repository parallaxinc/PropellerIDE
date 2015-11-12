TEMPLATE = lib
TARGET = zipper 
target.path = $${PREFIX}/bin

CONFIG += staticlib
CONFIG -= debug_and_release app_bundle

QT += gui widgets

SOURCES += \
    zip.cpp \
    zipper.cpp

HEADERS += \
    zconf.h \
    zipper.h \
    zipreader.h \
    zipwriter.h \
    zlib.h
