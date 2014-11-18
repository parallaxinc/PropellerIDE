! include( ../../common.pri ) {
    error( No common.pri file! )
}

TEMPLATE = lib
CONFIG += staticlib

SOURCES += qextserialport.cpp \
    qextserialenumerator.cpp

HEADERS  += qextserialport.h \
    qextserialenumerator.h \
    qextserialenumerator_p.h \
    qextserialport_p.h 


unix {
    SOURCES        += qextserialport_unix.cpp
}
unix:!macx:SOURCES += qextserialenumerator_unix.cpp
macx {
  SOURCES          += qextserialenumerator_osx.cpp
}
win32 {
  SOURCES          += qextserialport_win.cpp
  SOURCES          += qextserialenumerator_win.cpp
}
