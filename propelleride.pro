TEMPLATE  = subdirs

SUBDIRS = src

assets.path = $$PREFIX/share
assets.files = assets/*

INSTALLS += assets
