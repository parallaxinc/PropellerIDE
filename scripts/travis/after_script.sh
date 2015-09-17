#!/bin/bash
set -e

ls -R build/staging/
tree 

case "$PLATFORM" in
"osx")
    ;;
"linux")
    ;;
*)
    echo "Invalid PLATFORM"
    exit 1
    ;;
esac
