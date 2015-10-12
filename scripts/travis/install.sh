#!/bin/bash
set -e

git submodule update --init --recursive

case "$PLATFORM" in
"osx")
    brew install tree
    wget -4 http://lamestation.net/downloads/travis/qt5.5.0-mac-clang.tgz
    tar xzf qt5.5.0-mac-clang.tgz
    mv local/ /Users/travis/local/
    ;;
"linux")
#    wget -4 http://lamestation.net/downloads/travis/qt5.5.0-linux-g++.tgz
#    tar xzf qt5.5.0-linux-g++.tgz
#    mv local/ /home/travis/local/

    sudo apt-get install qt5-default libqt5serialport5-dev
    ;;
"rpi")
    # initial setup

    IMAGENAME=2015-05-05-raspbian-wheezy
    IMAGE=${IMAGENAME}.img
    ZIP=${IMAGENAME}.zip

    RPIHOME=$MNT/home/pi
    MNT=~/mnt

    # install dependencies

    sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid main restricted"
    sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid-updates main restricted"
    sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid universe"
    sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid-updates universe"
    sudo apt-get update
    sudo apt-get install qemu-user-static qemu-kvm kpartx

    # get and prepare Raspbian image

    wget -4 http://lamestation.net/downloads/travis/$ZIP
    unzip -o $ZIP

    # mount image

    mkdir -p $MNT 
    sudo kpartx -a -v $IMAGE
    sudo mount /dev/mapper/loop0p2 $MNT

    # set up arch chroot

    pushd scripts/travis/rpi

    mkdir -p $MNT/usr/bin
    sudo cp -vf ./inside-chroot.sh $MNT/
    sudo cp -vf ./inside-chroot-script.sh $MNT/
    sudo cp -vf /usr/bin/qemu-arm-static $MNT/usr/bin/
    sudo mount -o bind /dev  $MNT/dev
    sudo mount -o bind /proc $MNT/proc
    sudo mount -o bind /sys  $MNT/sys
    sudo mount --bind /home  $MNT/home
    sudo cp -vf /etc/network/interfaces $MNT/etc/network/interfaces
    sudo cp -vf /etc/resolv.conf $MNT/etc/resolv.conf 
    #sudo sed -e's/^/#/g' -i $MNT/etc/ld.so.preload
    #sudo echo 'deb http://mirrordirector.raspbian.org/raspbian/ jessie main contrib non-free rpi' >> $MNT/etc/apt/sources.list

    sudo su -c ./register-interpreter.sh root
    popd
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac

case "$PLATFORM" in
"linux"|"rpi")
    pushd $HOME
    git clone https://github.com/lamestation/packthing
    pushd packthing
    sudo pip install -r requirements.txt
    sudo python setup.py install
    popd
    popd
    ;;
"osx")
    mkdir -p $HOME/local/lib/python2.7/site-packages
    easy_install --prefix=$HOME/local pyyaml
    pushd $HOME
    git clone https://github.com/lamestation/packthing
    pushd packthing
    python setup.py install --prefix=$HOME/local
    popd
    popd
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac

case "$PLATFORM" in
"rpi")
    pushd scripts/travis/rpi
    echo "Entering chroot (install.sh)"
    pwd
    ls -l $MNT/
    tree
    sudo chroot $MNT/ /bin/bash -c "./inside-chroot.sh"
    popd
    ;;
"linux"|"osx")
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
