TEMPLATE  = subdirs

SUBDIRS = \
    zipper \
    propellermanager \
    projectview \
    memorymap \
    propterm \
    propelleride

propterm.depends = propellermanager
memorymap.depends = propellermanager projectview

propelleride.depends =  zipper \
                        memorymap \
                        propterm \
                        projectview \
                        propellermanager

memorymap.subdir = memorymap/src
projectview.subdir = projectview/src

