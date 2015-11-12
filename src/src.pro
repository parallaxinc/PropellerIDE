TEMPLATE  = subdirs

SUBDIRS = \
    zipper \
    memorymap/src \
    propterm \
    projectview/src \
    propellermanager \
    propelleride

propterm.depends = propellermanager
memorymap/src.depends = propellermanager

propelleride.depends =  zipper \
                        memorymap/src \
                        propterm \
                        projectview/src \
                        propellermanager \
