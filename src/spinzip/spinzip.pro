include( ../../propelleride.pri )

TEMPLATE = lib
TARGET = spinzip
target.path = $${PREFIX}/bin
CONFIG += staticlib

SOURCES += \
    zip.cpp \
    zipper.cpp

HEADERS += \
    zconf.h \
    zipper.h \
    zipreader.h \
    zipwriter.h \
    zlib.h
