include( ../../propelleride.pri )

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

INSTALLS += target

LIBS += -L$${OUT_PWD}/../qext/ -lqext
LIBS += -L$${OUT_PWD}/../spinzip/ -lspinzip

isEmpty(VERSION):VERSION = 0.0.0
VERSION = '\\"$${VERSION}\\"'
DEFINES += VERSION=\"$${VERSION}\"

SOURCES += main.cpp\
    mainwindow.cpp \
    $$files(mainwindow/*.cpp) \
    StatusDialog.cpp \
    Highlighter.cpp \
    treemodel.cpp \
    treeitem.cpp \
    Builder.cpp \
    SpinBuilder.cpp \
    PathSelector.cpp \
    PortListener.cpp \
    Preferences.cpp \
    PortConnectionMonitor.cpp \
    ReferenceTree.cpp \
    editor.cpp \
    SpinHighlighter.cpp \
    SpinParser.cpp \
    Terminal.cpp \
    Console.cpp \
    ColorScheme.cpp \
    ColorChooser.cpp \
    termprefs.cpp

HEADERS  += mainwindow.h \
    StatusDialog.h \
    Highlighter.h \
    treemodel.h \
    treeitem.h \
    Builder.h \
    SpinBuilder.h \
    PathSelector.h \
    PortListener.h \
    Preferences.h \
    PortConnectionMonitor.h \
    ReferenceTree.h \
    editor.h \
    SpinHighlighter.h \
    SpinParser.h \
    Sleeper.h \
    Terminal.h \
    Console.h \
    termprefs.h \
    ColorChooser.h \
    ColorScheme.h \
    templates/Singleton.h \

TRANSLATIONS += \
    translations/propelleride_zn.ts

OTHER_FILES +=

FORMS += \
    TermPrefs.ui \
    finder.ui

RESOURCES += \
    icons/icons.qrc

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
