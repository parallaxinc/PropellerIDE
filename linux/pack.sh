#!/bin/sh --
#
# create a .deb package
#

FOLDERS=`echo "propelleride/usr propelleride/usr/bin propelleride/opt propelleride/opt/propelleride propelleride/opt/propelleride/bin"`

# Show usage
usage()
{
  echo "Usage: $0 <path-to-PropellerIDE>"
  exit 1
}

# build package
build()
{
  SPKG=`echo ${SRC_PACKAGE}`
  SPKG=`echo ${SPKG} | sed 's/\.*\///g'`
  echo "Building debian package for ${SPKG}"

  echo "make propelleride folders"

  for DIR in ${FOLDERS} ; do
    mkdir -p $DIR
    if [ $? -ne 0 ]; then
      echo "Error making folder $DIR"
      exit 1
    fi
  done

  echo "Copy propelleride.sh"
  cp ${SRC_PACKAGE}/bin/propelleride.sh propelleride/usr/bin/propelleride
  if [ $? -ne 0 ]; then
    echo "Failure copying propelleride.sh"
    exit 1
  fi

  echo "Ensure executable"
  chmod u+x propelleride/usr/bin/propelleride
  if [ $? -ne 0 ]; then
    echo "Failure chmod u+x propelleride"
    exit 1
  fi

  echo "Copy PropellerIDE"
  cp ${SRC_PACKAGE}/bin/PropellerIDE propelleride/opt/propelleride/bin
  if [ $? -ne 0 ]; then
    echo "Failure copying PropellerIDE"
    exit 1
  fi

  echo "Copy openspin"
  cp ${SRC_PACKAGE}/bin/openspin propelleride/opt/propelleride/bin
  if [ $? -ne 0 ]; then
    echo "Failure copying openspin"
    exit 1
  fi

  echo "Copy p1load"
  cp ${SRC_PACKAGE}/bin/p1load propelleride/opt/propelleride/bin
  if [ $? -ne 0 ]; then
    echo "Failure copying p1load"
    exit 1
  fi

  echo "Copy License Info"
  cp -r ${SRC_PACKAGE}/license propelleride/opt/propelleride
  if [ $? -ne 0 ]; then
    echo "Failure copying license"
    exit 1
  fi

  echo "Copy Spin Library"
  cp -r ${SRC_PACKAGE}/spin propelleride/opt/propelleride
  if [ $? -ne 0 ]; then
    echo "Failure copying license"
    exit 1
  fi

  echo "Make .deb Package"
  VER=`echo ${SRC_PACKAGE} | sed 's/^.*\([0-9].*\)-\([0-9].*\)/\1\.\2/g'`
  NAM=`uname -n`
  CPU=`uname -m`
  if [ ${NAM} = raspberrypi ]; then
    CPU=armhf
  else
    if [ ${CPU} = i686 ]; then
      CPU=i386
    fi
    if [ ${CPU} = x86_64 ]; then
      CPU=amd64
    fi
  fi
  mkdir -p propelleride/DEBIAN/
  cat debian_control | sed "s/VERSION/${VER}/g" > temp1
  cat temp1 | sed "s/CPU/${CPU}/g" > propelleride/DEBIAN/control
  rm -f temp1
  dpkg-deb -b propelleride propelleride-${VER}-${CPU}.deb
  if [ $? -ne 0 ]; then
    echo "Failure making package"
    exit 1
  fi
}

cleanup()
{
    for DIR in ${FOLDERS} ; do
      rm -rf $DIR
      if [ $? -ne 0 ]; then
        echo "Error removing $DIR"
        exit 1
      fi
    done
}

if [ -z $1 ]
then
    usage
    exit 1
fi

case $1 in

  clean)
    echo "Clean files"
    cleanup
  ;;

  ?*)
  if [ x${1} != "x" ]; then
    SRC_PACKAGE=$1
    build
  else
    usage
  fi
  ;;

esac


exit 0
