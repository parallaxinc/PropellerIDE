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

!greaterThan(QT_MAJOR_VERSION, 4): {
    error("PropellerIDE requires Qt5.2 or greater")
}
!greaterThan(QT_MINOR_VERSION, 1): {
    error("PropellerIDE requires Qt5.2 or greater")
}

CONFIG -= qt debug_and_release app_bundle
CONFIG += console

INSTALLS += target

isEmpty(VERSION_ARG):VERSION_ARG = 0.0.0
VERSION_ARG = '\\"$${VERSION_ARG}\\"'
DEFINES += VERSION=\"$${VERSION_ARG}\"

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
