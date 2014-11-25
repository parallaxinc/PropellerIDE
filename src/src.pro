TEMPLATE  = subdirs

SUBDIRS = \
    prebuilt \
    qext \
    spinzip \
    p1load \
    openspin \
    propelleride \

propelleride.depends = qext spinzip
