#!/bin/bash

set -e

sudo apt-get update
sudo apt-get install qt5-default libqt5serialport5-dev

pushd /home/travis/packthing
python setup.py install
popd

uname -a
qmake -v
packthing -h

pushd /home/travis/build/parallaxinc/PropellerIDE/
packthing -j4 rpi
mv build/staging/propelleride-*.deb .
popd
