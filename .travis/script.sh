#!/bin/bash
set -e

BUILD_VERSION="$TRAVIS_BRANCH"
if [[ -n "$TRAVIS_TAG" ]]
then
    BUILD_VERSION="$TRAVIS_TAG"
fi

case "$PLATFORM" in
"osx")
    qmake -v
    packthing -h

    packthing -j4 --version $BUILD_VERSION dmg
    mv build/staging/propelleride-*.dmg .
    ;;
"linux")
    qmake -v
    packthing -h

    packthing -j4 run
    mv build/staging/propelleride-*.run .

    fakeroot packthing -j4 --version $BUILD_VERSION deb
    mv build/staging/propelleride-*.deb .
    ;;
"rpi")
    sudo chroot $MNT uname -a
    sudo chroot $MNT qmake -v
    sudo chroot $MNT packthing -h

    sudo packthing --checkout

    sudo chroot $MNT bash -c "cd /home/travis/build/parallaxinc/PropellerIDE/ && \
                                        packthing -j4 deb --version $BUILD_VERSION --arch armhf && \
                                        mv build/staging/propelleride-*.deb ."

    sudo chown `whoami`:`groups | sed -r 's/ .*//g'` propelleride-*.deb
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
