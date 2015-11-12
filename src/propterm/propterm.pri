QT += gui widgets serialport
CONFIG -= app_bundle debug_and_release

INCLUDEPATH += ../../propellermanager/include/
LIBS += -L../../propellermanager/lib/  -lpropellermanager

win32-msvc* {
	PRE_TARGETDEPS += ../../propellermanager/lib/propellermanager.lib
} else {
	PRE_TARGETDEPS += ../../propellermanager/lib/libpropellermanager.a
}

RESOURCES += \
    ../icons/propterm/propterm.qrc \
