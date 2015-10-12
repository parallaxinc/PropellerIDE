#!/bin/bash

pushd /home/travis/build/parallaxinc/PropellerIDE/
packthing -j4 rpi
mv build/staging/propelleride-*.deb .
popd
