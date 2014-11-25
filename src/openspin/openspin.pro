isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = openspin
target.path = $${PREFIX}/bin
INSTALLS += target

CONFIG -= qt debug_and_release

SOURCES += \
    $$files(OpenSpin/PropellerCompiler/*.cpp) \
    $$files(OpenSpin/SpinSource/*.cpp) \
