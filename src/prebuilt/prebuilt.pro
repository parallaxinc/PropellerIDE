isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = aux
CONFIG -= debug_and_release

extras.files += \
    ../../LICENSE.GPL \
    ../../LICENSE.MIT \
    ../../COPYING \
    ../../AUTHORS \
    ../../library \
    ../../doc

unix:!macx {
    prebuilt.files += $$files(unix/*)
    prebuilt.path = $${PREFIX}/
    extras.path = $${PREFIX}/share/propelleride
}
macx {
    prebuilt.files += $$files(macx/*)
    prebuilt.path = $${PREFIX}/
    extras.path = $${PREFIX}/Resources
}
win32 {
    prebuilt.files += $$files(win32/*)
    prebuilt.path = $${PREFIX}/
    extras.path = $${PREFIX}/
}

INSTALLS += prebuilt extras
