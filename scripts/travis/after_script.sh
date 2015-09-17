#!/bin/bash
set -e

ls -R build/staging/
tree 

case "$PLATFORM" in
"osx")
    for f in build/staging/mac/PropellerIDE.app/Contents/MacOS/*; do echo $f; otool -L $f; done
    ;;
"linux")
    for f in build/staging/debian/propelleride/usr/bin/*; do echo $f; ldd $f; done
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
