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

TARGET = propelleride
TEMPLATE = app

CONFIG  += exceptions

DEFINES += IDEVERSION=0
DEFINES += MINVERSION=20

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
    EzBuilder.cpp \
    SpinBuilder.cpp \
    qextserialport.cpp \
    PortListener.cpp \
    properties.cpp \
    qextserialenumerator.cpp \
    PortConnectionMonitor.cpp \
    editor.cpp \
    highlightSpin.cpp \
    spinparser.cpp \
    terminal.cpp \
    console.cpp \
    termprefs.cpp \
    zipper.cpp \
    zip.cpp \

HEADERS  += mainwindow.h \
    StatusDialog.h \
    highlighter.h \
    treemodel.h \
    treeitem.h \
    EzBuilder.h \
    SpinBuilder.h \
    qextserialport.h \
    qextserialenumerator.h \
    PortListener.h \
    properties.h \
    PortConnectionMonitor.h \
    qextserialenumerator_p.h \
    editor.h \
    highlightSpin.h \
    spinparser.h \
    propertycolor.h \
    Sleeper.h \
    terminal.h \
    console.h \
    termprefs.h \
    zipper.h \
    zconf.h \
    zlib.h \
    zipreader.h \
    zipwriter.h \
    qextserialport_p.h \
    qtversion.h

OTHER_FILES +=

FORMS += \
    TermPrefs.ui

RESOURCES += \
    resources.qrc

unix {
    SOURCES        += qextserialport_unix.cpp
    LIBS           += -lz
}
unix:!macx:SOURCES += qextserialenumerator_unix.cpp
macx {
  SOURCES          += qextserialenumerator_osx.cpp
  LIBS             += -framework IOKit -framework CoreFoundation
}
win32 {
  SOURCES          += qextserialport_win.cpp
  SOURCES          += qextserialenumerator_win.cpp
  LIBS             += -lsetupapi
  LIBS             += -L$$PWD -lzlib1
}
