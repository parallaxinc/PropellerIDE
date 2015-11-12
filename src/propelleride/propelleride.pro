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

INCLUDEPATH += ../zipper/
INCLUDEPATH += ../projectview/include/
INCLUDEPATH += ../propellermanager/include/
INCLUDEPATH += ../propterm/include/
INCLUDEPATH += ../memorymap/include/

LIBS += -L../zipper/ -lzipper
LIBS += -L../projectview/lib/  -lprojectview
LIBS += -L../propellermanager/lib/  -lpropellermanager
LIBS += -L../propterm/lib/  -lpropterm
LIBS += -L../memorymap/lib/ -lmemorymap -lQHexEdit

win32-msvc* {
	PRE_TARGETDEPS += ../zipper/zipper.lib
	PRE_TARGETDEPS += ../projectview/lib/projectview.lib
	PRE_TARGETDEPS += ../propellermanager/lib/propellermanager.lib
	PRE_TARGETDEPS += ../propterm/lib/propterm.lib
	PRE_TARGETDEPS += ../memorymap/lib/QHexEdit.lib
	PRE_TARGETDEPS += ../memorymap/lib/memorymap.lib
} else {
	PRE_TARGETDEPS += ../zipper/libzipper.a
	PRE_TARGETDEPS += ../projectview/lib/libprojectview.a
	PRE_TARGETDEPS += ../propellermanager/lib/libpropellermanager.a
	PRE_TARGETDEPS += ../propterm/lib/libpropterm.a
	PRE_TARGETDEPS += ../memorymap/lib/libQHexEdit.a
	PRE_TARGETDEPS += ../memorymap/lib/libmemorymap.a
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
    logging_ide.cpp \

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
    logging_ide.h \

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

