NAME			:=	propelleride

DIR				:=	$(shell pwd)
DIR_SRC			:=	$(DIR)/src
DIR_BUILD		:=	$(DIR)/build

DIR_STAGING		:=	$(DIR)/staging
DIR_OUT			:=	$(DIR_STAGING)

DIR_DIST		:=	$(DIR)/dist

DIR_COMMON		:=	$(DIR)/common

ISCC			:=	iscc
QMAKE			:=	qmake -r

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

build:
	cd $(DIR_SRC); $(QMAKE) PREFIX=$(DIR_OUT); $(MAKE)

copy: build
	cd $(DIR_SRC); $(MAKE) install

clean_staging:
	rm -rf $(DIR_STAGING)

clean: clean_staging
	cd $(DIR_SRC); $(QMAKE); $(MAKE) clean

deb: DIR_OUT := $(DIR_STAGING)/usr
deb: clean_staging copy
	mkdir -p $(DIR_STAGING)/DEBIAN/ ; \
	cp -f $(DIR_DIST)/control $(DIR_STAGING)/DEBIAN/control ; \
	sed -e "s/VERSION/$(VERSION)/" \
		-e "s/CPU/$(CPU)/" \
		-i $(DIR_STAGING)/DEBIAN/control ; \
	dpkg-deb -b $(DIR_STAGING) $(DIR_DIST)/propelleride-$(VERSION)-$(CPU).deb


win: build
	$(ISCC) //dMyAppVersion=$(VERSION) "$(DIR_DIST)/installer.iss"


mac: DIR_OUT := "$(DIR_STAGING)/PropellerIDE.app/Contents"
mac: clean_staging copy
	

