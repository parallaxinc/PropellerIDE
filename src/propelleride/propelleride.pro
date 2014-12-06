include( ../../propelleride.pri )

TEMPLATE = app
TARGET = propelleride

macx {
    target.path = $${PREFIX}/MacOS
}
unix:!macx {
    target.path = $${PREFIX}/bin
}
win32 {
    target.path = $${PREFIX}/
}

INSTALLS += target

CONFIG -= app_bundle

LIBS += -L$${OUT_PWD}/../qext/ -lqext
LIBS += -L$${OUT_PWD}/../spinzip/ -lspinzip

SOURCES += main.cpp\
    mainwindow.cpp \
    $$files(mainwindow/*.cpp) \
    StatusDialog.cpp \
    Highlighter.cpp \
    treemodel.cpp \
    treeitem.cpp \
    Builder.cpp \
    SpinBuilder.cpp \
    PortListener.cpp \
    Preferences.cpp \
    PortConnectionMonitor.cpp \
    editor.cpp \
    SpinHighlighter.cpp \
    SpinParser.cpp \
    terminal.cpp \
    console.cpp \
    termprefs.cpp

HEADERS  += mainwindow.h \
    StatusDialog.h \
    Highlighter.h \
    treemodel.h \
    treeitem.h \
    Builder.h \
    SpinBuilder.h \
    PortListener.h \
    Preferences.h \
    PortConnectionMonitor.h \
    editor.h \
    SpinHighlighter.h \
    SpinParser.h \
    colors.h \
    Sleeper.h \
    terminal.h \
    console.h \
    termprefs.h \

OTHER_FILES +=

FORMS += \
    TermPrefs.ui

RESOURCES += \
    icons/icons.qrc

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
