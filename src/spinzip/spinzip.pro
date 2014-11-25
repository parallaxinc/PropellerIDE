include( ../../propelleride.pri )

TEMPLATE = lib
TARGET = spinzip
target.path = $${PREFIX}/bin
CONFIG += staticlib

SOURCES += $$files(*.cpp)
HEADERS  += $$files(*.h)
