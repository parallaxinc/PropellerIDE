#-------------------------------------------------
#
# Project created by QtCreator 2011-07-31T06:21:35
#
#-------------------------------------------------

#-------------------------------------------------
#
# TODO list
#
# - clean up mainwindow class ;-)
#
#-------------------------------------------------

QT += core
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): {
    QT -= gui
    QT += widgets
    QT += printsupport
    DEFINES += QT5
}

TARGET = PropellerIDE
TEMPLATE = app

CONFIG  += exceptions

DEFINES += QEXTSERIALPORT_LIB
DEFINES += IDEVERSION=0
DEFINES += MINVERSION=19

# ENABLE_AUTO_ENTER turns on AUTO-INDENT
DEFINES += ENABLE_AUTO_ENTER
# SPIN_AUTOCOMPLETE turns on spinny-sense
DEFINES += SPIN_AUTOCOMPLETE
# Enable ZIP feature
DEFINES += ENABLE_ZIP
# Enable Enter NL/CR option
DEFINES += ENABLE_ENTER_ISNL

# Use a reliable external loader
DEFINES += EXTERNAL_SPIN_LOADER

#enable simple graphing terminal tabs
#DEFINES += ENABLE_TERM_TABS

SOURCES += main.cpp\
    mainwindow.cpp \
    StatusDialog.cpp \
    highlighter.cpp \
    treemodel.cpp \
    treeitem.cpp \
    XBasicModel.cpp \
    EzBuilder.cpp \
    SpinBuilder.cpp \
    XBasicBuilder.cpp \
    qextserialport.cpp \
    PortListener.cpp \
    terminal.cpp \
    properties.cpp \
    console.cpp \
    qextserialenumerator.cpp \
    PortConnectionMonitor.cpp \
    editor.cpp \
    highlightSpin.cpp \
    spinparser.cpp \
    highlightC.cpp \
    terminal2.cpp \
    termprefs.cpp \
    zipper.cpp \
    zip.cpp \
    p1loader.cpp \
    p1load.c \
    graphline.cpp \
    clock.cpp \
    graphwidget.cpp

HEADERS  += mainwindow.h \
    StatusDialog.h \
    highlighter.h \
    treemodel.h \
    treeitem.h \
    XBasicModel.h \
    EzBuilder.h \
    SpinBuilder.h \
    XBasicBuilder.h \
    qextserialport.h \
    qextserialenumerator.h \
    PortListener.h \
    terminal.h \
    properties.h \
    console.h \
    PortConnectionMonitor.h \
    qextserialenumerator_p.h \
    editor.h \
    highlightSpin.h \
    spinparser.h \
    highlightC.h \
    propertycolor.h \
    Sleeper.h \
    terminal2.h \
    termprefs.h \
    zipper.h \
    zconf.h \
    zlib.h \
    zipreader.h \
    zipwriter.h \
    qextserialport_p.h \
    p1loader.h \
    osint.h \
    p1load.h \
    graphline.h \
    clock.h \
    graphwidget.h \
    qtversion.h

OTHER_FILES +=

FORMS += \
    TermPrefs.ui

RESOURCES += \
    resources.qrc

unix {
    SOURCES        += osint_linux.c
    SOURCES        += qextserialport_unix.cpp
    LIBS           += -lz
}
unix:!macx:SOURCES += qextserialenumerator_unix.cpp
macx {
  SOURCES          += qextserialenumerator_osx.cpp
  LIBS             += -framework IOKit -framework CoreFoundation
}
win32 {
  RC_FILE          = myapp.rc

  HEADERS          += ioapi.h iowin32.h
  SOURCES          += enumcom.c
  SOURCES          += osint_mingw.c
  SOURCES          += qextserialport_win.cpp
  SOURCES          += qextserialenumerator_win.cpp
  LIBS             += -lsetupapi
  LIBS             += -L$$PWD -lzlib1
}
