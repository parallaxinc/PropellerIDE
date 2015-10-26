TEMPLATE  = subdirs

SUBDIRS = \
    spinzip \
    memorymap \
    terminal \
    projectview \
    propellermanager \
    propelleride

terminal.depends = propellermanager
memorymap.depends = propellermanager

propelleride.depends = spinzip memorymap terminal projectview propellermanager
