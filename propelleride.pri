isEmpty(PREFIX):PREFIX = /usr/local
isEmpty(LIBDIR):LIBDIR = $${PREFIX}/lib
isEmpty(RPATH):RPATH = yes

QT += gui

greaterThan(QT_MAJOR_VERSION, 4): {
    QT += widgets
    DEFINES += QT5
}

INCLUDEPATH += . ..

CONFIG -= debug_and_release

DEFINES += IDEVERSION=0
DEFINES += MINVERSION=20
