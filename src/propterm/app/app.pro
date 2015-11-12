include("../propterm.pri")

TARGET = propterm
TEMPLATE = app
DESTDIR = ../bin/

SOURCES += \
    main.cpp

INCLUDEPATH += ../include/
LIBS += -L../lib/  -lpropterm

win32-msvc* {
	PRE_TARGETDEPS += ../lib/propterm.lib
} else {
	PRE_TARGETDEPS += ../lib/libpropterm.a
}

