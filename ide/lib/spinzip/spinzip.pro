! include( ../../common.pri ) {
    error( No common.pri file! )
}

TEMPLATE = lib
CONFIG += staticlib

SOURCES += *.cpp 
HEADERS  += *.h
