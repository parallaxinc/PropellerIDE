isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = aux
CONFIG -= debug_and_release
INSTALLS += first
QMAKE_EXTRA_TARGETS = first
QMAKE_CLEAN =

first.commands = \
    git submodule init; \
    git submodule update; \
    cd OpenSpin; \
    make;
first_clean.commands = cd OpenSpin && make clean

win32 {
    first.files = OpenSpin/openspin.exe
} else {
    first.files = OpenSpin/openspin
}

first.path = $${PREFIX}/bin


