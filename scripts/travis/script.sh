#!/bin/bash
set -e

qmake -v
packthing -h

case "$PLATFORM" in
"osx")
    packthing -j4 $PACKTHING
    ;;
"linux")
    fakeroot packthing -j4 $PACKTHING
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
