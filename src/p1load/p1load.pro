isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = p1load
target.path = $${PREFIX}/bin
INSTALLS += target

CONFIG -= qt debug_and_release
CONFIG += console

SOURCES += p1load.c

unix {
    DEFINES += LINUX
    SOURCES += osint_linux.c
}
macx {
    DEFINES += MACOSX
    SOURCES += osint_linux.c
}
win32 {
    DEFINES += MINGW
    LIBS    += -lsetupapi
    SOURCES += osint_mingw.c enumcom.c
}
