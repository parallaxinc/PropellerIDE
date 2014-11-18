! include( ../common.pri ) {
    error( No common.pri file! )
}

TEMPLATE = app

TARGET = ../propelleride

LIBS += -L../qext -lqext

SOURCES += main.cpp\
    mainwindow.cpp \
    mainwindow/*.cpp \
    StatusDialog.cpp \
    highlighter.cpp \
    treemodel.cpp \
    treeitem.cpp \
    EzBuilder.cpp \
    SpinBuilder.cpp \
    PortListener.cpp \
    preferences.cpp \
    PortConnectionMonitor.cpp \
    editor.cpp \
    highlightSpin.cpp \
    spinparser.cpp \
    terminal.cpp \
    console.cpp \
    termprefs.cpp \
    spinzip/*.cpp 

HEADERS  += mainwindow.h \
    StatusDialog.h \
    highlighter.h \
    treemodel.h \
    treeitem.h \
    EzBuilder.h \
    SpinBuilder.h \
    PortListener.h \
    preferences.h \
    PortConnectionMonitor.h \
    editor.h \
    highlightSpin.h \
    spinparser.h \
    colors.h \
    Sleeper.h \
    terminal.h \
    console.h \
    termprefs.h \
    qtversion.h \
    spinzip/*.h

OTHER_FILES +=

FORMS += \
    TermPrefs.ui

RESOURCES += \
    resources.qrc

unix {
    LIBS           += -lz
}
macx {
  LIBS             += -framework IOKit -framework CoreFoundation
}
win32 {
  LIBS             += -lsetupapi
  LIBS             += -L$$PWD -lzlib1
}
