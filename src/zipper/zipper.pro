QT += gui widgets

TEMPLATE = lib
TARGET = zipper 

CONFIG += staticlib
CONFIG -= debug_and_release app_bundle

SOURCES += \
    zip.cpp \
    zipper.cpp

HEADERS += \
    zconf.h \
    zipper.h \
    zipreader.h \
    zipwriter.h \
    zlib.h
