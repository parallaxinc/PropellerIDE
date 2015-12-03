INCLUDEPATH += $$PWD/
LIBS += -L$$PWD/ -lzipper

win32-msvc* {
	PRE_TARGETDEPS += $$PWD/zipper.lib
} else {
	PRE_TARGETDEPS += $$PWD/libzipper.a
}

