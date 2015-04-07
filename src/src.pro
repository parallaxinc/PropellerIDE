TEMPLATE  = subdirs

SUBDIRS = \
    spinzip \
    memorymap \
    projectview \
    propelleride

propelleride.depends = spinzip memorymap projectview
