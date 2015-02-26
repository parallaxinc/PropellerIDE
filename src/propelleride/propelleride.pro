TEMPLATE = app
TARGET = propelleride
QT += gui widgets serialport

!greaterThan(QT_MAJOR_VERSION, 4): {
    error("PropellerIDE requires Qt5.2 or greater")
}
!greaterThan(QT_MINOR_VERSION, 1): {
    error("PropellerIDE requires Qt5.2 or greater")
}

CONFIG += console
CONFIG -= debug_and_release app_bundle

INCLUDEPATH += . ..

LIBS += -L$${OUT_PWD}/../spinzip/ -lspinzip

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    find.cpp \
    Highlighter.cpp \
    treemodel.cpp \
    treeitem.cpp \
    clickable.cpp \
    PathSelector.cpp \
    Preferences.cpp \
    PortConnectionMonitor.cpp \
    ReferenceTree.cpp \
    editor.cpp \
    status.cpp \
    SpinHighlighter.cpp \
    SpinParser.cpp \
    ColorScheme.cpp \
    ColorChooser.cpp \
    FileManager.cpp \
    BuildManager.cpp \

HEADERS  += \
    mainwindow.h \
    Highlighter.h \
    treemodel.h \
    treeitem.h \
    clickable.h \
    PathSelector.h \
    Preferences.h \
    PortConnectionMonitor.h \
    ReferenceTree.h \
    editor.h \
    SpinHighlighter.h \
    SpinParser.h \
    status.h \
    ColorChooser.h \
    ColorScheme.h \
    templates/Singleton.h \
    FileManager.h \
    BuildManager.h \

OTHER_FILES +=

FORMS += \
    forms/mainwindow.ui \
    forms/TermPrefs.ui \
    forms/finder.ui \
    forms/status.ui

RESOURCES += \
    icons/icons.qrc \
    fonts/fonts.qrc \
    themes/themes.qrc \

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

