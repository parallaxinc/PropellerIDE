TEMPLATE = app

SOURCES += p1load.c
CONFIG -= qt

unix {
    DEFINES += LINUX
    SOURCES += osint_linux.c
}
macx {
    DEFINES += MACOSX
    SOURCES += osint_mingw.c enumcom.c
}
win32 {
    DEFINES += MINGW
    LIBS    += -lsetupapi
}
