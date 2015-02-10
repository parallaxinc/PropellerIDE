TEMPLATE  = subdirs

SUBDIRS = \
    prebuilt \
    spinzip \
    openspin \
    propelleride

!win32 {
    SUBDIRS += p1load
}

propelleride.depends = spinzip
