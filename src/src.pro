TEMPLATE  = subdirs

SUBDIRS = \
    qext \
    spinzip \
    p1load \
    openspin \
    propelleride \

propelleride.depends = qext spinzip
