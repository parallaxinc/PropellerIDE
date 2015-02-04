OS:=$(shell uname)
CC=gcc
CXX=g++
ifeq ($(OS),Darwin)
	CFLAGS+=-Wall -g
else
	CFLAGS+=-Wall -g -static
endif
CXXFLAGS += $(CFLAGS)


LIBNAME=libopenspin.a
SRCDIR=.
OBJ=$(SRCDIR)/BlockNestStackRoutines.o \
	$(SRCDIR)/CompileDatBlocks.o \
	$(SRCDIR)/CompileExpression.o \
	$(SRCDIR)/CompileInstruction.o \
	$(SRCDIR)/CompileUtilities.o \
	$(SRCDIR)/DistillObjects.o \
	$(SRCDIR)/Elementizer.o \
	$(SRCDIR)/ErrorStrings.o \
	$(SRCDIR)/ExpressionResolver.o \
	$(SRCDIR)/InstructionBlockCompiler.o \
	$(SRCDIR)/StringConstantRoutines.o \
	$(SRCDIR)/SymbolEngine.o \
	$(SRCDIR)/Utilities.o \
	$(SRCDIR)/PropellerCompiler.o \


all: $(LIBNAME) Makefile


$(LIBNAME): $(OBJ)
	$(AR) rs $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<


clean:
	rm -rf $(SRCDIR)/*.o $(LIBNAME)
