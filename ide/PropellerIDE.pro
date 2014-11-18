TEMPLATE = subdirs
SUBDIRS = lib

# build must be last:
CONFIG += ordered
SUBDIRS += main
