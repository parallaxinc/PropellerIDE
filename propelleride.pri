isEmpty(PREFIX):PREFIX = /usr/local

QT += gui

greaterThan(QT_MAJOR_VERSION, 4): {
    QT += widgets
    DEFINES += QT5
}

INCLUDEPATH += . ..

CONFIG -= debug_and_release app_bundle
