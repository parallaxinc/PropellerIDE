TEMPLATE = app
TARGET = propelleride
QT += gui widgets serialport

!greaterThan(QT_MAJOR_VERSION, 4): {
    error("PropellerIDE requires Qt5.2 or greater")
}
!greaterThan(QT_MINOR_VERSION, 1): {
    error("PropellerIDE requires Qt5.2 or greater")
}

CONFIG -= debug_and_release app_bundle

INCLUDEPATH += ../spinzip/
INCLUDEPATH += ../projectview/
INCLUDEPATH += ../propellermanager/src/lib/
INCLUDEPATH += ../propterm/lib/
INCLUDEPATH += ../memorymap/memorymap/
INCLUDEPATH += ../memorymap/

LIBS += -L../spinzip/ -lspinzip
LIBS += -L../projectview/  -lprojectview
LIBS += -L../propellermanager/src/lib/  -lpropellermanager
LIBS += -L../propterm/lib/  -lpropterm
LIBS += -L../memorymap/memorymap/  -lmemorymap
LIBS += -L../memorymap/QHexEdit/ -lQHexEdit

win32-msvc* {
	PRE_TARGETDEPS += ../spinzip/spinzip.lib
	PRE_TARGETDEPS += ../projectview/projectview.lib
	PRE_TARGETDEPS += ../propellermanager/src/lib/propellermanager.lib
	PRE_TARGETDEPS += ../propterm/lib/propterm.lib
	PRE_TARGETDEPS += ../memorymap/QHexEdit/QHexEdit.lib
	PRE_TARGETDEPS += ../memorymap/memorymap/memorymap.lib
} else {
	PRE_TARGETDEPS += ../spinzip/libspinzip.a
	PRE_TARGETDEPS += ../projectview/libprojectview.a
	PRE_TARGETDEPS += ../propellermanager/src/lib/libpropellermanager.a
	PRE_TARGETDEPS += ../propterm/lib/libpropterm.a
	PRE_TARGETDEPS += ../memorymap/QHexEdit/libQHexEdit.a
	PRE_TARGETDEPS += ../memorymap/memorymap/libmemorymap.a
}

SOURCES += \
    buildmanager.cpp \
    colorscheme.cpp \
    colorchooser.cpp \
    editor.cpp \
    filemanager.cpp \
    finder.cpp \
    highlighter.cpp \
    language.cpp \
    main.cpp \
    mainwindow.cpp \
    pathselector.cpp \
    preferences.cpp \
    buildstatus.cpp \

HEADERS  += \
    buildmanager.h \
    clickable.h \
    colorchooser.h \
    colorscheme.h \
    editor.h \
    filemanager.h \
    finder.h \
    highlighter.h \
    language.h \
    mainwindow.h \
    pathselector.h \
    preferences.h \
    buildstatus.h \
    templates/Singleton.h \

OTHER_FILES +=

FORMS += \
    forms/about.ui \
    forms/mainwindow.ui \
    forms/finder.ui \
    forms/buildstatus.ui \

RESOURCES += \
    fonts/fonts.qrc \
    icons/icons.qrc \
    languages/languages.qrc \
    themes/themes.qrc \
    ../projectview/icons/projectview/projectview.qrc \
    ../propterm/icons/propterm/propterm.qrc \

CODECFORTR = UTF-8

unix {
    LIBS            += -lz
}
macx {
    LIBS            += -framework IOKit -framework CoreFoundation
}
win32 {
    LIBS            += -L$$PWD -lz
    RC_FILE         = propelleride.rc
}

TRANSLATIONS += \
    translations/propelleride.ts \ 
    translations/propelleride_es.ts \
    translations/propelleride_zn.ts \
    translations/propelleride_fake.ts \

