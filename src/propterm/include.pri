INCLUDEPATH += $$PWD/include/
LIBS += -L$$PWD/lib/  -lpropterm

win32-msvc* {
	PRE_TARGETDEPS += $$PWD/lib/propterm.lib
} else {
	PRE_TARGETDEPS += $$PWD/lib/libpropterm.a
}

include($$PWD/icons/include.pri)

