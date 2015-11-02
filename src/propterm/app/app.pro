CONFIG -= app_bundle debug_and_release

TARGET = propterm
TEMPLATE = app

SOURCES += \
    main.cpp

INCLUDEPATH += ../lib/
LIBS += -L../lib/  -lpropterm

win32-msvc* {
	PRE_TARGETDEPS += ../lib/propterm.lib
} else {
	PRE_TARGETDEPS += ../lib/libpropterm.a
}

include("../propterm.pri")
