#!/bin/bash
set -e

git submodule update --init --recursive

# install Qt

case "$PLATFORM" in
"osx")
    brew update
    brew install qt5
    brew link qt5 --force
    ;;
"linux")
    wget -4 http://lamestation.net/downloads/travis/qt5.5.0-linux-g++.tgz
    tar xzf qt5.5.0-linux-g++.tgz
    mv local/ /home/travis/local/
    export PATH=/home/travis/local/bin:$PATH
    export LD_LIBRARY_PATH=/home/travis/local/lib:$LD_LIBRARY_PATH
    ls /home/travis/local/bin
    ls /home/travis/local/lib
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
qmake -v

# install packthing

mkdir -p $HOME/local/lib/python2.7/site-packages
easy_install --prefix=$HOME/local packthing

packthing -h
