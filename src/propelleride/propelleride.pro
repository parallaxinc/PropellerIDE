TEMPLATE = app
TARGET = propelleride
QT += gui widgets serialport printsupport

UI_DIR = .ui/
MOC_DIR = .moc/
OBJECTS_DIR = .obj/
RCC_DIR = .qrc/

!greaterThan(QT_MAJOR_VERSION, 4): {
    error("PropellerIDE requires Qt5.2 or greater")
}
!greaterThan(QT_MINOR_VERSION, 1): {
    error("PropellerIDE requires Qt5.2 or greater")
}

CONFIG -= debug_and_release app_bundle

include("../propterm/include.pri")
include("../projectview/include.pri")
include("../memorymap/include.pri")
include("../zipper/include.pri")
include("../propellermanager/include.pri")

SOURCES += \
    buildmanager.cpp \
    colorscheme.cpp \
    colorchooser.cpp \
    filemanager.cpp \
    finder.cpp \
    highlighter.cpp \
    language.cpp \
    main.cpp \
    mainwindow.cpp \
    newfromtemplate.cpp \
    templateicon.cpp \
    pathselector.cpp \
    preferences.cpp \
    logging.cpp \

HEADERS  += \
    setup.h \
    buildmanager.h \
    clickable.h \
    colorchooser.h \
    colorscheme.h \
    filemanager.h \
    finder.h \
    highlighter.h \
    language.h \
    mainwindow.h \
    newfromtemplate.h \
    templateicon.h \
    pathselector.h \
    preferences.h \
    templates/Singleton.h \
    logging.h \

INCLUDEPATH += compilers/

SOURCES += \
    compilers/externalcompiler.cpp \

HEADERS += \
    compilers/compiler.h \
    compilers/externalcompiler.h \

INCLUDEPATH += views/

SOURCES += \
    views/editorview.cpp \

HEADERS += \
    views/editorview.h \


OTHER_FILES +=

FORMS += \
    forms/about.ui \
    forms/mainwindow.ui \
    forms/preferences.ui \
    forms/pathselector.ui \
    forms/finder.ui \
    forms/buildmanager.ui \
    forms/newfromtemplate.ui \
    forms/templateicon.ui \

RESOURCES += \
    fonts/fonts.qrc \
    icons/icons.qrc \
    languages/languages.qrc \
    themes/themes.qrc \
    config/config.qrc \

CODECFORTR = UTF-8

unix {
    LIBS += -lz
}
macx {
    LIBS += -framework IOKit -framework CoreFoundation
}
win32 {
    LIBS += -L$$PWD -lz
    RC_FILE = propelleride.rc
}

TRANSLATIONS += \
    translations/propelleride.ts \ 
    translations/propelleride_es.ts \
    translations/propelleride_zn.ts \
    translations/propelleride_fake.ts \

