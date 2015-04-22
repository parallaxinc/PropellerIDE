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
LIBS += -L../spinzip/ -lspinzip
PRE_TARGETDEPS += ../spinzip/libspinzip.a

INCLUDEPATH += ../memorymap/memorymap/
LIBS += -L../memorymap/memorymap/  -lmemorymap
PRE_TARGETDEPS += ../memorymap/QHexEdit/libQHexEdit.a

INCLUDEPATH += ../memorymap/
LIBS += -L../memorymap/QHexEdit/ -lQHexEdit
PRE_TARGETDEPS += ../memorymap/memorymap/libmemorymap.a

INCLUDEPATH += ../projectview/
LIBS += -L../projectview/  -lprojectview
PRE_TARGETDEPS += ../projectview/libprojectview.a

SOURCES += \
    BuildManager.cpp \
    ColorScheme.cpp \
    ColorChooser.cpp \
    editor.cpp \
    FileManager.cpp \
    Finder.cpp \
    Highlighter.cpp \
    Language.cpp \
    main.cpp \
    mainwindow.cpp \
    PathSelector.cpp \
    Preferences.cpp \
    status.cpp \

HEADERS  += \
    BuildManager.h \
    clickable.h \
    ColorChooser.h \
    ColorScheme.h \
    editor.h \
    FileManager.h \
    Finder.h \
    Highlighter.h \
    Language.h \
    mainwindow.h \
    PathSelector.h \
    portmonitor.h \
    Preferences.h \
    status.h \
    templates/Singleton.h \

OTHER_FILES +=

FORMS += \
    forms/about.ui \
    forms/mainwindow.ui \
    forms/finder_flat.ui \
    forms/status.ui \

RESOURCES += \
    fonts/fonts.qrc \
    icons/icons.qrc \
    languages/languages.qrc \
    themes/themes.qrc \
    ../projectview/icons/projectview/projectview.qrc \

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

