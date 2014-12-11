isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = openspin

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

CONFIG -= qt debug_and_release app_bundle
CONFIG += console

SOURCES += \
    $$files(repo/PropellerCompiler/*.cpp) \
    $$files(repo/SpinSource/*.cpp) \
