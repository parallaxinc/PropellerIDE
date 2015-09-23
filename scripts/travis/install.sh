#!/bin/bash
set -e

git submodule update --init --recursive

case "$PLATFORM" in
"osx")
    brew install tree qt5
    ;;
"linux")
#    wget -4 http://lamestation.net/downloads/travis/qt5.5.0-linux-g++.tgz
#    tar xzf qt5.5.0-linux-g++.tgz
#    mv local/ /home/travis/local/
#    ;;
    sudo add-apt-repository "deb mirror://mirrors.ubuntu.com/mirrors.txt utopic main"
    sudo apt-get update
    sudo apt-get install qt5-default qt5-qmake qtbase5-dev-tools qttools5-dev qtchooser qtbase5-dev libgles2-mesa-dev libqt5gui5 libqt5serialport5-dev libqt5widgets5 libgles2-mesa libgl1-mesa-dev libgl1-mesa-glx libglapi-mesa libegl1-mesa-dev libpango-1.0-0
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac

case "$PLATFORM" in
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
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
