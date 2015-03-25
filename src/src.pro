TEMPLATE  = subdirs

SUBDIRS = \
    spinzip \
    memorymap \
    propelleride

propelleride.depends = spinzip memorymap
