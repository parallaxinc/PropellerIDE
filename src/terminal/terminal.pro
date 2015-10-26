QT += widgets serialport

TARGET = propterm
TEMPLATE = app
CONFIG -= app_bundle debug_and_release

INCLUDEPATH += ../propellermanager/src/lib/
LIBS += -L../propellermanager/src/lib/  -lpropellermanager

win32-msvc* {
	PRE_TARGETDEPS += ../propellermanager/src/lib/propellermanager.lib
} else {
	PRE_TARGETDEPS += ../propellermanager/src/lib/libpropellermanager.a
}

SOURCES += \
    main.cpp \
    propterm.cpp \
    console.cpp

HEADERS += \
    propterm.h \
    console.h

FORMS += \
    propterm.ui \

RESOURCES += \
    icons/propterm/propterm.qrc \
