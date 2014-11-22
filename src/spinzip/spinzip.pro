! include( ../../propelleride.pri ) {
    error( No propelleride.pri file! )
}

TEMPLATE = lib
CONFIG += staticlib

SOURCES += *.cpp 
HEADERS  += *.h
