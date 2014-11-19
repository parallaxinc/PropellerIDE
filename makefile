NAME			:=	propelleride

DIR				:=	$(shell pwd)
DIR_OPENSPIN	:=	$(DIR)/openspin
DIR_IDE			:=	$(DIR)/ide
DIR_SPINLIB		:=	$(DIR)/spin
DIR_LOADER		:=	$(DIR)/loader
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


VERSION			:=	$(shell echo $(shell grep -r VERSION= $(DIR_IDE)/common.pri \
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



build_all: build_ide build_openspin build_loader

copy_all: clean_staging copy_root copy_common copy_bin 

clean: clean_staging
	make -C $(DIR_LOADER) clean
	make -C $(DIR_OPENSPIN) clean
	make -C $(DIR_IDE) clean

clean_staging:
	rm -rf $(DIR_STAGING)

build_ide:
	cd $(DIR_IDE); qmake PropellerIDE.pro; make $(JOBS)

build_openspin:
	cd $(DIR_OPENSPIN); make

build_loader:
	cd $(DIR_LOADER); make

copy_bin:
	mkdir -p $(DIR_BIN)
	cp $(DIR_IDE)/$(NAME) $(DIR_BIN)
	cp $(DIR_OPENSPIN)/openspin $(DIR_BIN)
	cp $(DIR_LOADER)/p1load $(DIR_BIN)

copy_root:
	rsync -ru $(DIR_PREBUILT)/* $(DIR_STAGING)

copy_common:
	rsync -ru $(DIR_COMMON)/* $(DIR_TARGET)

deb: build_all copy_all
	sed -e "s/VERSION/$(VERSION)/" \
		-e "s/CPU/$(CPU)/" \
		-i $(DIR_STAGING)/DEBIAN/control
	dpkg-deb -b $(DIR_STAGING) propelleride-$(VERSION)-$(CPU).deb
