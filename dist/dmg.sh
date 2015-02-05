#!/bin/bash

source="$1"
title="$2"
target="$3"
size=$(echo "$((`du -s ${source} | awk '{print $1}'`+1000))")
echo ${size}

tmpdevice="$(dirname ${source})/pack.temp.dmg"

hdiutil create -srcfolder "${source}" -volname "${title}" -fs HFS+ \
      -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${size}k ${tmpdevice}

device=$(hdiutil attach -readwrite -noverify -noautoopen "${tmpdevice}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

background="../gfx/newspin.png" # this needs a more robust mechanism
applicationName="PropellerIDE.app"

VOLUME_DIR="/Volumes/${title}/.background/"
mkdir "${VOLUME_DIR}"
cp -f dist/${background} "${VOLUME_DIR}"

echo '
   tell application "Finder"
     tell disk "'${title}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {400, 100, 900, 434}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 72
           set background picture of theViewOptions to file ".background:'${background}'"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "'${applicationName}'" of container window to {105, 180}
           set position of item "Applications" of container window to {395, 180}
           update without registering applications
           delay 5
           close
     end tell
   end tell
' | osascript

chmod -Rf go-w /Volumes/"${title}"
sync
sync
hdiutil detach "${device}"
hdiutil convert "${tmpdevice}" -format UDZO -imagekey zlib-level=9 -o "${target}"
rm -f "${tmpdevice}"
