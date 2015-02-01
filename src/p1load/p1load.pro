isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = p1load

macx {
    target.path = $${PREFIX}/MacOS
}
unix:!macx {
    target.path = $${PREFIX}/share/propelleride/bin
}
win32 {
    target.path = $${PREFIX}/
}

INSTALLS += target

CONFIG -= qt debug_and_release app_bundle
CONFIG += console

SOURCES += \
    repo/p1load.c \
    repo/ploader.c

HEADERS += \
    repo/ploader.h

unix:!macx {
    DEFINES += LINUX
    SOURCES += \
        repo/osint_linux.c

    equals(CPU, armhf) {
        DEFINES += RASPBERRY_PI
        SOURCES += \
            repo/gpio_sysfs.c
    }
}
macx {
    DEFINES += MACOSX
    SOURCES += \
        repo/osint_linux.c
}
win32 {
    DEFINES += MINGW
    LIBS    += -lsetupapi
    SOURCES += \
        repo/osint_mingw.c \
        repo/enumcom.c
}
