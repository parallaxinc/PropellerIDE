#!/bin/bash
set -e

git submodule update --init --recursive

case "$PLATFORM" in
"osx")
    brew install tree qt5
    ;;
"linux")
    wget -4 http://lamestation.net/downloads/travis/qt5.5.0-linux-g++.tgz
    tar xzf qt5.5.0-linux-g++.tgz
    mv local/ /home/travis/local/
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac

mkdir -p $HOME/local/lib/python2.7/site-packages
pushd $HOME
git clone https://github.com/lamestation/packthing
pushd packthing
python setup.py install --prefix=$HOME/local
popd
popd
#mkdir -p $HOME/local/lib/python2.7/site-packages
#easy_install --prefix=$HOME/local packthing

qmake -v
packthing -h
