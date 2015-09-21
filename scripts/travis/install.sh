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

pushd $HOME
git clone https://github.com/lamestation/packthing
pushd packthing
sudo python setup.py install
popd
popd

qmake -v
packthing -h
