isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = aux
CONFIG -= debug_and_release

unix:!macx {
    prebuilt.files += $$files(unix/*)
    prebuilt.path = $${PREFIX}/
}
macx {
    prebuilt.files += $$files(macx/*)
    prebuilt.path = $${PREFIX}/
}
win32 {
    prebuilt.files += $$files(win32/*)
    prebuilt.path = $${PREFIX}/
}

INSTALLS += prebuilt
