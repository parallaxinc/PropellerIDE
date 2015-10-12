#!/bin/bash

#sudo apt-get update
#sudo apt-get install qt5-default

pushd /home/travis/packthing
python setup.py install
popd

uname -a
qmake -v
packthing -h
