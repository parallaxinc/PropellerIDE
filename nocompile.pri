isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = prebuilt
CONFIG -= qt sdk separate_debug_info gdb_dwarf_index
QT =
LIBS =
QMAKE_LINK = @: IGNORE THIS LINE
OBJECTS_DIR =
win32:CONFIG -= embed_manifest_exe

