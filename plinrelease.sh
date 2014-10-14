#!/bin/sh -x
#
# This script is intended for making production linux PropellerIDE packages
# and uses an install methodology deemed appropriate by Parallax.
#
# If you want to use the old packaging methodology, run linrelease.sh
#
# This packager does not include any dynamic support libraries as it is
# assumed that linux users will be able to find the necessary pieces.
#
# To use this release script:
#
# 1) QtSDK Desktop qmake must be in your PATH
# 2) workspace must exist in spinside folder ala:
#   hg clone https://jsdenson@code.google.com/p/propsideworkspace/ workspace
# 
#
NAME=PropellerIDE
PKG=${NAME}.zip
BUILD=build
SETUP="setup.sh"
PROPIDE="propelleride.sh"
OPENSPIN=openspin
SPINLIB=./spin
LOADER=loader

SETUPSH="./linux/${SETUP}"
SIDERSH="./linux/${PROPIDE}"

#
# We only provide PropellerIDE, openspin, qt libs, and spin source in this packager
#
LIBAUDIO="/usr/lib/libaudio.so.2"
LIBAUDIO2="/usr/lib/x86_64-linux-gnu/libaudio.so.2"
SPINLIB="./spin"

QMAKE=`which qmake`

CLEAN=$1

UARCH=`arch`
UNAME=`uname -n`
if [ `uname -s` = "msys" ]; then
    JOBS=
else
    ISARM=`echo ${UARCH} | grep -i "arm"`
    if [ x${ISARM} != "x" ]; then
        JOBS=-j2
    else
        JOBS=-j6
    fi
fi

#
# remove package
#
if [ -e ${PKG} ]; then
   rm -rf ${PKG}
fi

#
# build openspin for release
#
if [ -e ./${OPENSPIN} ]; then
    cd ${OPENSPIN}
    if [ x$CLEAN != xnoclean ]; then
        make clean
    fi
    make
else
    echo "An openspin repository is required for this program."
    echo "Please use git to setup a repository with: "
    echo "  git clone https://github.com/reltham/OpenSpin.git openspin"
    exit 1
fi

if test $? != 0; then
   echo "openspin make failed."
   exit 1
fi

cd ..

mkdir -p ${BUILD}

#
# build PropellerIDE for release
#
cp -r ./ide/* ${BUILD}
DIR=`pwd`
cd ${BUILD}
qmake -config ${BUILD}
if test $? != 0; then
   echo "qmake config failed."
   exit 1
fi

if [ x$CLEAN != xnoclean ]; then
    make clean
    make ${JOBS}
fi

if test $? != 0; then
   echo "PropellerIDE make failed."
   exit 1
fi
cd ${DIR}

cd ${LOADER}
make clean
make
if test $? != 0; then
   echo "p1load make failed."
   exit 1
fi
cd ${DIR}


# extract version from propside.pro
#
SEDCMD=`sed -n 's/VERSION=.*$/&/p' ${BUILD}/EzIDE.pro | cut -d"=" -f3`
VERSION=`echo ${SEDCMD}`
VERSION=`echo ${VERSION} | sed 's/ /-/g'`
VERSION=`echo ${VERSION} | sed 's/\r//g'`
VERSION="${NAME}-${VERSION}"
echo "Creating Version ${VERSION}"

# create package direcotry
#
rm -rf ${VERSION}
mkdir -p ${VERSION}
if test $? != 0; then
   echo "mkdir ${VERSION} failed."
   exit 1
fi

cp -r ./license ${VERSION}
if test $? != 0; then
   echo "copy license info failed."
   exit 1
fi

cp -rf ${SPINLIB} ${VERSION}
if test $? != 0; then
   echo "copy spin library failed."
   exit 1
fi

mkdir -p ${VERSION}/bin
cp -r ${BUILD}/${NAME} ${VERSION}/bin
if test $? != 0; then
   echo "copy ${NAME} failed."
   exit 1
fi

cp ./${OPENSPIN}/${OPENSPIN} ${VERSION}/bin
cp ./${LOADER}/p1load ${VERSION}/bin

#cp ./PropellerIDE-User-Guide.pdf ${VERSION}/bin
#if test $? != 0; then
#   echo "copy User Guide failed."
#   exit 1
#fi

#cp -r ${BUILD}/translations ${VERSION}
#if test $? != 0; then
#   echo "copy translations failed."
#   exit 1
#fi

ls -RF ${VERSION} | grep ":"

MYLDD=`ldd ${BUILD}/${NAME} | grep libQt | awk '{print $3}'`
LIBS=`echo $MYLDD`

cp -f ${LIBS} ${VERSION}/bin
if test $? != 0; then
   echo "copy ${LIBS} failed."
   exit 1
fi

MYLDD=`ldd ${BUILD}/${NAME} | grep libQt | awk '{print $3}'`
LIBS=`echo $MYLDD`

cp -f ${LIBS} ${VERSION}/bin
if test $? != 0; then
   echo "copy ${LIBS} failed."
   exit 1
fi

AULDD=`ldd ${BUILD}/${NAME} | grep libaudio | awk '{print $3}'`
LIBAUDIO=`echo $AULDD`

if [ ${LIBAUDIO}X != X ]; then
   cp -f ${LIBAUDIO} ${VERSION}/bin
   if test $? != 0; then
      if test $? != 0; then
         echo "Can't find libaudio...."
      fi
   fi
fi

cp ${SETUPSH} ${VERSION}
cp ${SIDERSH} ${VERSION}/bin

chmod u+x ${VERSION}/${SETUP}
chmod 755 ${VERSION}/bin/${PROPIDE}

# pack-up a bzip tarball for distribution
tar -cjf ${VERSION}.${UARCH}.${UNAME}-linux.tar.bz2 ${VERSION}

exit 0
