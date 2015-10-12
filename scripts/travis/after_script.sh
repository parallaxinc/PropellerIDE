#!/bin/bash
set -e

tree build/staging/

case "$PLATFORM" in
"osx")
    for f in build/staging/mac/PropellerIDE.app/Contents/MacOS/*; do echo $f; otool -L $f; done
    ;;
"linux"|"rpi")
    for f in build/staging/debian/propelleride/usr/bin/*; do echo $f; ldd $f; done
    lintian build/staging/propelleride*.deb
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
