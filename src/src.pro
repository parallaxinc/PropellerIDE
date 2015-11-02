TEMPLATE  = subdirs

SUBDIRS = \
    spinzip \
    memorymap \
    propterm \
    projectview \
    propellermanager \
    propelleride

propterm.depends = propellermanager
memorymap.depends = propellermanager

propelleride.depends = spinzip memorymap propterm projectview propellermanager
