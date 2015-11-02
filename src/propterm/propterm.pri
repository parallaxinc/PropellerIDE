QT += widgets serialport

VPATH += $$PWD/ $$PWD/..
INCLUDEPATH += $$PWD/ $$PWD/..
INCLUDEPATH += ../../propellermanager/src/lib/
LIBS += -L../../propellermanager/src/lib/  -lpropellermanager

win32-msvc* {
	PRE_TARGETDEPS += ../../propellermanager/src/lib/propellermanager.lib
} else {
	PRE_TARGETDEPS += ../../propellermanager/src/lib/libpropellermanager.a
}

RESOURCES += \
    icons/propterm/propterm.qrc \
