NAME			:=	propelleride

ISCC			:=	/c/Program\ Files\ \(x86\)/Inno\ Setup\ 5/ISCC.exe 

DIR				:=	$(shell pwd)
DIR_SRC			:=	$(DIR)/src
DIR_STAGING		:=	$(DIR)/staging
DIR_COMMON		:=	$(DIR)/common

deb: OS 			:=	unix
deb: DIR_PREBUILT	:=	$(DIR)/prebuilt/$(OS)
deb: DIR_BIN		:=	$(DIR_STAGING)/usr/bin
deb: DIR_TARGET		:=	$(DIR_STAGING)/usr/share/$(NAME)

win: OS 			:=	win32
win: DIR_PREBUILT	:=	$(DIR)/prebuilt/$(OS)

mac: OS 			:=	macx
mac: DIR_PREBUILT	:=	$(DIR)/prebuilt/$(OS)


VERSION			:=	$(shell echo $(shell grep -r VERSION= propelleride.pri \
					| cut -d'=' -f3 \
					| sed -e 's/[\r]//g') \
					| sed -e 's/ /./g')

# if CPU (uname -m) equals...
ifeq ($(shell uname -n),raspberrypi)		# if Raspberry Pi
	CPU := armhf
else
	ifeq ($(shell uname -m),i686)			# if i686
		CPU := i386
	else ifeq ($(shell uname -m),x86_64)	# if x64
		CPU := amd64
	else
		CPU := $(shell uname -m)
	endif
endif

all: build

help:
	@echo "Usage:    make [TYPE]"
	@echo
	@echo "Enter package type to build. Options:"
	@echo
	@echo "   deb    debian package"
	@echo "   win    windows installer (not yet available)"
	@echo "   mac    mac bundle        (not yet available)"
	@echo
	@echo "   no parameter builds only the binaries"

checkout:
	git submodule init
	git submodule update

copy_all: clean_staging copy_root copy_common copy_bin 

build:
	qmake -r; $(MAKE) -f Makefile

clean_staging:
	rm -rf $(DIR_STAGING)

clean: clean_staging
	qmake -r; $(MAKE) -f Makefile clean

copy:
	mkdir -p $(DIR_BIN); cp -u `find $(DIR_SRC) -executable -type f` $(DIR_BIN)
	rsync -ru $(DIR_PREBUILT)/* $(DIR_STAGING)
	rsync -ru doc $(DIR_STAGING)/usr/share/propelleride
	rsync -ru library $(DIR_STAGING)/usr/share/propelleride

deb: copy
	sed -e "s/VERSION/$(VERSION)/" \
		-e "s/CPU/$(CPU)/" \
		-i $(DIR_STAGING)/DEBIAN/control
	dpkg-deb -b $(DIR_STAGING) propelleride-$(VERSION)-$(CPU).deb

win: build
	$(ISCC) //dMyAppVersion=$(VERSION) "installer.iss"
