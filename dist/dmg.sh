#!/bin/bash

source="$1"
title="$2"
size=$(echo "$((`du -s ${source} | awk '{print $1}'`+1000))")
echo ${size}

tmpdevice="$(dirname ${source})/pack.temp.dmg"

hdiutil create -srcfolder "${source}" -volname "${title}" -fs HFS+ \
      -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${size}k ${tmpdevice}

device=$(hdiutil attach -readwrite -noverify -noautoopen "${tmpdevice}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

mkdir "/Volumes/${title}/.background/"
cp -f dist/propelleride.png "/Volumes/${title}/.background/"

echo '
   tell application "Finder"
     tell disk "'${title}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {400, 100, 885, 430}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 72
           set background picture of theViewOptions to file ".background:'propelleride.png'"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "'${applicationName}'" of container window to {100, 100}
           set position of item "Applications" of container window to {375, 100}
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
hdiutil convert "${tmpdevice}" -format UDZO -imagekey zlib-level=9 -o "$(dirname ${source})/PropellerIDE.dmg"
rm -f "${tmpdevice}"
