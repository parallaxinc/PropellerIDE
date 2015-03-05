isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = propelleride

macx {
    target.path = $${PREFIX}/MacOS
}
unix:!macx {
    target.path = $${PREFIX}/share/propelleride/bin
}
win32 {
    target.path = $${PREFIX}/
}

!greaterThan(QT_MAJOR_VERSION, 4): {
    error("PropellerIDE requires Qt5.2 or greater")
}
!greaterThan(QT_MINOR_VERSION, 1): {
    error("PropellerIDE requires Qt5.2 or greater")
}
QT += gui widgets

CONFIG += console
CONFIG -= debug_and_release app_bundle

INSTALLS += target

INCLUDEPATH += . ..

LIBS += -L$${OUT_PWD}/../qext/ -lqext
LIBS += -L$${OUT_PWD}/../spinzip/ -lspinzip

isEmpty(VERSION_ARG):VERSION_ARG = 0.0.0
VERSION_ARG = '\\"$${VERSION_ARG}\\"'
DEFINES += VERSION=\"$${VERSION_ARG}\"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    find.cpp \
    Highlighter.cpp \
    treemodel.cpp \
    treeitem.cpp \
    clickable.cpp \
    PathSelector.cpp \
    PortListener.cpp \
    Preferences.cpp \
    PortConnectionMonitor.cpp \
    ReferenceTree.cpp \
    editor.cpp \
    status.cpp \
    StatusDialog.cpp \
    SpinHighlighter.cpp \
    SpinParser.cpp \
    Terminal.cpp \
    Console.cpp \
    ColorScheme.cpp \
    ColorChooser.cpp \
    termprefs.cpp \
    FileManager.cpp \
    BuildManager.cpp \

HEADERS  += \
    mainwindow.h \
    Highlighter.h \
    treemodel.h \
    treeitem.h \
    clickable.h \
    PathSelector.h \
    PortListener.h \
    Preferences.h \
    PortConnectionMonitor.h \
    ReferenceTree.h \
    editor.h \
    SpinHighlighter.h \
    SpinParser.h \
    status.h \
    StatusDialog.h \
    Terminal.h \
    Console.h \
    termprefs.h \
    ColorChooser.h \
    ColorScheme.h \
    templates/Singleton.h \
    FileManager.h \
    BuildManager.h \

TRANSLATIONS += \
    translations/propelleride_zn.ts

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
