TEMPLATE  = subdirs

SUBDIRS = \
    qext \
    spinzip \
    p1load \
    OpenSpin \
    propelleride \

propelleride.depends = qext spinzip
