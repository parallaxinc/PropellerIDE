TEMPLATE  = subdirs

SUBDIRS = \
    spinzip \
    memorymap \
    projectview \
    propellermanager \
    propelleride

propelleride.depends = spinzip memorymap projectview propellermanager
