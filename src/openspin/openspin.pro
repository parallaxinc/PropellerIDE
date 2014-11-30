isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = openspin

macx {
    target.path = $${PREFIX}/MacOS
} else {
    target.path = $${PREFIX}/bin
}

INSTALLS += target

CONFIG -= qt debug_and_release app_bundle
CONFIG += console

SOURCES += \
    $$files(repo/PropellerCompiler/*.cpp) \
    $$files(repo/SpinSource/*.cpp) \
