isEmpty(PREFIX):PREFIX = /usr/local

TEMPLATE = app
TARGET = openspin

macx {
    target.path = $${PREFIX}/MacOS
}
unix:!macx {
    target.path = $${PREFIX}/share/propelleride/bin
}
win32 {
    target.path = $${PREFIX}/
}

INSTALLS += target

CONFIG -= debug_and_release app_bundle
CONFIG += console

INCLUDEPATH += repo repo/PropellerCompiler repo/SpinSource

HEADERS += \
    repo/PropellerCompiler/CompileUtilities.h \
    repo/PropellerCompiler/Elementizer.h \
    repo/PropellerCompiler/ErrorStrings.h \
    repo/PropellerCompiler/PropellerCompiler.h \
    repo/PropellerCompiler/PropellerCompilerInternal.h \
    repo/PropellerCompiler/SymbolEngine.h \
    repo/PropellerCompiler/Utilities.h \
    repo/SpinSource/flexbuf.h \
    repo/SpinSource/objectheap.h \
    repo/SpinSource/pathentry.h \
    repo/SpinSource/preprocess.h \
    repo/SpinSource/textconvert.h

SOURCES += \
    openspin.cpp \
    repo/PropellerCompiler/BlockNestStackRoutines.cpp \
    repo/PropellerCompiler/CompileDatBlocks.cpp \
    repo/PropellerCompiler/CompileExpression.cpp \
    repo/PropellerCompiler/CompileInstruction.cpp \
    repo/PropellerCompiler/CompileUtilities.cpp \
    repo/PropellerCompiler/DistillObjects.cpp \
    repo/PropellerCompiler/Elementizer.cpp \
    repo/PropellerCompiler/ErrorStrings.cpp \
    repo/PropellerCompiler/ExpressionResolver.cpp \
    repo/PropellerCompiler/InstructionBlockCompiler.cpp \
    repo/PropellerCompiler/PropellerCompiler.cpp \
    repo/PropellerCompiler/StringConstantRoutines.cpp \
    repo/PropellerCompiler/SymbolEngine.cpp \
    repo/PropellerCompiler/Utilities.cpp \
    repo/SpinSource/flexbuf.cpp \
    repo/SpinSource/objectheap.cpp \
    repo/SpinSource/pathentry.cpp \
    repo/SpinSource/preprocess.cpp \
    repo/SpinSource/textconvert.cpp
