TEMPLATE  = subdirs

SUBDIRS = \
    prebuilt \
    qext \
    spinzip \
    openspin \
    propelleride

!win32 {
    SUBDIRS += p1load
}

propelleride.depends = qext spinzip
