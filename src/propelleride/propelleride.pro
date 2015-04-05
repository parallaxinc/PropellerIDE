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
CONFIG += debug


LIBS += -L$${OUT_PWD}/../spinzip/ -lspinzip
INCLUDEPATH += . ..

LIBS += -L../memorymap/memorymap/  -lmemorymap
LIBS += -L../memorymap/QHexEdit -lQHexEdit
INCLUDEPATH += ../memorymap/memorymap/ ../memorymap/

INCLUDEPATH += ../projectview/

SOURCES += \
    BuildManager.cpp \
    clickable.cpp \
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
    PortConnectionMonitor.cpp \
    Preferences.cpp \
    ReferenceTree.cpp \
    SpinParser.cpp \
    status.cpp \
    treemodel.cpp \
    treeitem.cpp \

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
    PortConnectionMonitor.h \
    Preferences.h \
    ReferenceTree.h \
    SpinParser.h \
    status.h \
    treeitem.h \
    treemodel.h \
    templates/Singleton.h \

OTHER_FILES +=

FORMS += \
    forms/about.ui \
    forms/mainwindow.ui \
    forms/finder.ui \
    forms/finder_flat.ui \
    forms/status.ui \

RESOURCES += \
    fonts/fonts.qrc \
    icons/icons.qrc \
    languages/languages.qrc \
    themes/themes.qrc \




SOURCES += \
    ../projectview/parser.cpp \
    ../projectview/projectview.cpp \

HEADERS += \
    ../projectview/parser.h \
    ../projectview/projectview.h \

FORMS += \
    ../projectview/projectview.ui \

RESOURCES += \
    ../projectview/icons/projectview/projectview.qrc \


CODECFORTR = UTF-8

unix {
    LIBS            += -lz
}
macx {
    LIBS            += -framework IOKit -framework CoreFoundation
}
win32 {
    LIBS            += -lsetupapi
    LIBS            += -L$$PWD -lz
    RC_FILE         = propelleride.rc
}

TRANSLATIONS += \
    translations/propelleride.ts \ 
    translations/propelleride_es.ts \
    translations/propelleride_zn.ts \
    translations/propelleride_fake.ts \

