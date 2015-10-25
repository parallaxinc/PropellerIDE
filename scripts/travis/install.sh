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

    sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install qt5-default libqt5serialport5-dev
    ;;
"rpi")
    sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid universe"
    sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid-updates universe"
    sudo apt-get update
    sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install qemu-user-static

    FOLDER=travis-debian-jessie-armhf
    TAR=${FOLDER}.tgz
    wget -4 http://lamestation.net/downloads/travis/$TAR
    sudo tar xzf $TAR
    sudo mv -v $FOLDER $MNT
    sudo mount --bind /home  $MNT/home

    sudo cp -vf /etc/network/interfaces $MNT/etc/network/interfaces
    sudo cp -vf /etc/resolv.conf $MNT/etc/resolv.conf 
    sudo cp -vf /usr/bin/qemu-arm-static $MNT/usr/bin/
#    sudo chroot $MNT apt-get update
#    sudo chroot $MNT apt-get install qt5-default libqt5serialport5-dev
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac

# packthing installation

case "$PLATFORM" in
"rpi")
    pushd $HOME
    git clone https://github.com/lamestation/packthing
    pushd packthing
    sudo pip install -r requirements.txt
    sudo python setup.py install
    popd
    popd
    sudo chroot $MNT bash -c "cd /home/travis/packthing && \
                                pip install -r requirements.txt && \
                                python setup.py install"
    ;;
"linux")
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
