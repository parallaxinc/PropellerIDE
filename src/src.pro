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
memorymap/src.depends = projectview 

propelleride.depends =  zipper \
                        memorymap/src \
                        propterm \
                        projectview/src \
                        propellermanager \
