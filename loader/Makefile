## ##################################
##
## Loader makefile
##
## ##################################


# attempt to auto-detect the OS
#
UNAME=$(if $(findstring _,$(shell uname -s)),$(shell uname -o),$(shell uname -s))

# OS Types
#
OSS=Msys Linux Darwin
OS+=$(foreach OT, $(OSS), $(findstring $(OT), $(UNAME)))

CFLAGS += $(if $(findstring Darwin,$(OS)),-DMACOSX)
TGT    += $(if $(findstring Darwin,$(OS)),p1load)
SRC    += $(if $(findstring Darwin,$(OS)),osint_linux.c)

CFLAGS += $(if $(findstring Linux,$(OS)),-DLINUX)
TGT    += $(if $(findstring Linux,$(OS)),p1load)
SRC    += $(if $(findstring Linux,$(OS)),osint_linux.c)

CFLAGS += $(if $(findstring Msys,$(OS)),-DMINGW)
TGT    += $(if $(findstring Msys,$(OS)),p1load.exe)
LIBS   += $(if $(findstring Msys,$(OS)),-lsetupapi)
SRC    += $(if $(findstring Msys,$(OS)),osint_mingw.c)
SRC    += $(if $(findstring Msys,$(OS)),enumcom.c)

CC=gcc
CFLAGS += -Wall -g

SRC+=p1load.c
OBJ:=$(SRC:.c=.o)

all: Makefile $(OBJ) $(TGT)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

$(TGT): $(OBJ) $(OSLIB)
	$(CC) $(OBJ) $(OSLIB) -o $(TGT) $(LIBS)

clean:
	rm -f *.o $(TGT)
