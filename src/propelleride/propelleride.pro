! include( ../../propelleride.pri ) {
    error( No propelleride.pri file! )
}

TEMPLATE = app

LIBS += -L../qext -L../spinzip -lqext -lspinzip

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
    Preferences.cpp \
    PortConnectionMonitor.cpp \
    editor.cpp \
    highlightSpin.cpp \
    SpinParser.cpp \
    terminal.cpp \
    console.cpp \
    termprefs.cpp

HEADERS  += mainwindow.h \
    StatusDialog.h \
    highlighter.h \
    treemodel.h \
    treeitem.h \
    EzBuilder.h \
    SpinBuilder.h \
    PortListener.h \
    Preferences.h \
    PortConnectionMonitor.h \
    editor.h \
    highlightSpin.h \
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
    icons/icons.qrc \
    fonts/fonts.qrc

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
