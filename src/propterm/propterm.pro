TEMPLATE  = subdirs

SUBDIRS = \
    src \
    app

app.depends = src

unix|macx
{
    count(PREFIX,0):PREFIX=/usr/local

    libs.files = lib/*.a lib/*.so*
    bins.files = bin/*
    includes.files = include/*

    libs.path = $$PREFIX/lib/
    bins.path = $$PREFIX/bin/
    includes.path = $$PREFIX/include

    INSTALLS += bins libs includes
}

win32
{
    all.files = lib/* bin/* include/*
    all.path = $$PREFIX/
}
