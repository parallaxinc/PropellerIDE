#!/bin/bash
set -e


case "$PLATFORM" in
"osx")
    tree build/staging/
    for f in build/staging/mac/PropellerIDE.app/Contents/MacOS/*; do echo $f; otool -L $f; done
    ;;
"linux")
    ls -R build/staging/
    for f in build/staging/debian/propelleride/usr/bin/*; do echo $f; ldd $f; done
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
