#!/bin/bash
set -e

case "$PLATFORM" in
"osx")
    packthing -j4 $PACKTHING
    ;;
"linux")
    fakeroot packthing -j4 $PACKTHING
    ;;
"linux-apt")
    sudo packthing -j4 $PACKTHING
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
