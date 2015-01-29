NAME			:=	propelleride
TITLE			:=	PropellerIDE

DIR				:=	$(shell pwd)
DIR_SRC			:=	$(DIR)/src
DIR_BUILD		:=	$(DIR)/build

DIR_STAGING		:=	$(DIR)/staging
DIR_OUT			:=	$(DIR_STAGING)

DIR_DIST		:=	$(DIR)/dist

DIR_COMMON		:=	$(DIR)/common

ISCC			:=	iscc
QMAKE_OPTS		+=  -r
QMAKE			:=	qmake $(QMAKE_OPTS)

VERSION := $(shell git describe --tags --long)
ifeq ($(VERSION),)
	VERSION := 0.0.0-phony
endif

ifeq ($(shell uname -m),i686)			# if i686
	CPU := i386
else ifeq ($(shell uname -m),x86_64)	# if x64
	CPU := amd64
else
	CPU := $(shell uname -m)
endif

all: build

help:
	@echo "Usage:    make [TYPE]"
	@echo
	@echo "Enter package type to build. Options:"
	@echo
	@echo "   win    windows installer"
	@echo "   mac    mac bundle"
	@echo "   deb    debian package"
	@echo "   rpi    debian package for Raspberry Pi"
	@echo
	@echo "   no parameter builds only the binaries"

checkout:
	git submodule init
	git submodule update

build:
	cd $(DIR_SRC); $(QMAKE) VERSION_ARG=$(VERSION) PREFIX=$(DIR_OUT); $(MAKE)

copy: build
	cd $(DIR_SRC); $(MAKE) install

clean_staging:
	rm -rf $(DIR_STAGING)

clean: clean_staging
	cd $(DIR_SRC); $(QMAKE); $(MAKE) clean

deb: DIR_OUT := $(DIR_STAGING)/$(NAME)/usr
deb: clean_staging copy
	mkdir -p $(DIR_STAGING)/$(NAME)/DEBIAN/ ; \
	cp -f $(shell ldd $(DIR_OUT)/bin/$(NAME) | grep "libQt" | awk '{print $$3}') $(DIR_OUT)/share/$(NAME)/bin/; \
	cp -f $(DIR_DIST)/control $(DIR_STAGING)/$(NAME)/DEBIAN/control ; \
	sed -e "s/VERSION/$(VERSION)/" \
		-e "s/CPU/$(CPU)/" \
		-i $(DIR_STAGING)/$(NAME)/DEBIAN/control ; \
	dpkg-deb -b $(DIR_STAGING)/$(NAME) $(DIR_STAGING)/$(NAME)-$(VERSION)-$(CPU).deb

rpi: CPU := armhf
rpi: deb

win: DIR_OUT := "$(DIR_STAGING)/$(NAME)"
win: clean_staging copy
	cd $(DIR_OUT); \
	windeployqt $(NAME).exe; \
	$(ISCC) //dMyAppVersion=$(VERSION) "$(DIR_DIST)/installer.iss"


mac: DIR_OUT := "$(DIR_STAGING)/$(NAME).app/Contents"
mac: clean_staging copy
	cd $(DIR_STAGING) ; \
	macdeployqt $(DIR_STAGING)/$(TITLE).app ; \
	cp -f $(DIR_DIST)/Info.plist $(DIR_OUT)
	$(DIR_DIST)/dmg.sh $(DIR_STAGING)/$(TITLE).app $(TITLE) $(DIR_STAGING)/$(NAME)-$(VERSION)-$(CPU).dmg
