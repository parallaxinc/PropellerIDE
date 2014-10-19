NAME			:=	PropellerIDE

DIR				:=	$(shell pwd)
DIR_OPENSPIN	:=	$(DIR)/openspin
DIR_IDE			:=	$(DIR)/ide
DIR_SPINLIB		:=	$(DIR)/spin
DIR_LOADER		:=	$(DIR)/loader

DIR_ROOT		:=	$(DIR)/root
DIR_STAGING		:=	$(DIR)/staging
DIR_BIN			:=	$(DIR_STAGING)/bin


VERSION			:=	$(shell echo `grep -r VERSION= $(DIR_IDE)/PropellerIDE.pro \
					| cut -d'=' -f3 \
					| cut -d' ' -f 1` \
					| sed -e 's/ /./g')

ARCH			:=	$(shell arch)
COMPNAME		:=	$(shell uname -n)
OS				:=	$(shell uname -s)

PACKAGE			:=  ${NAME}-${VERSION}-${ARCH}-${OS}

ifeq ($(shell uname -s),msys)		# if Windows
	JOBS :=
else
	ifneq ($(filter arm%,$(ARCH)),) # if ARM
		JOBS := -j2
	else							# not ARM
		JOBS := -j6
	endif
endif

all: build_ide build_openspin build_loader copy_all build_debian

clean: clean_staging
	make -C $(DIR_LOADER) clean
	make -C $(DIR_OPENSPIN) clean
	make -C $(DIR_IDE) clean

clean_staging:
	rm -rf $(DIR_STAGING)

build_ide:
	cd $(DIR_IDE); qmake $(NAME).pro; make $(JOBS)

build_openspin:
	cd $(DIR_OPENSPIN); make

build_loader:
	cd $(DIR_LOADER); make

get_version:


copy_all: clean_staging copy_root copy_bin copy_libs

copy_bin:
	mkdir -p $(DIR_BIN)
	cp $(DIR_IDE)/$(NAME) $(DIR_BIN)
	cp $(DIR_OPENSPIN)/openspin $(DIR_BIN)
	cp $(DIR_LOADER)/p1load $(DIR_BIN)

copy_root:
	rsync -ru $(DIR_ROOT)/* $(DIR_STAGING)

copy_libs:
	cp `ldd $(DIR_IDE)/$(NAME) | grep 'libQt\|libz' | awk '{print $$3}'` $(DIR_BIN)

build_debian:
	mv $(DIR_STAGING) ${PACKAGE}/
	tar -cvzf ${PACKAGE}.tar ${PACKAGE}/
	gzip -f ${PACKAGE}.tar
	@echo ${PACKAGE}
