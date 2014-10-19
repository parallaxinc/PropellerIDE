#!/bin/sh
#---------------------------------------------------------------
#
# PropellerIDE user setup for Linux.
#
# This script assumes you have root access.
#
# To install PropellerIDE and Propeller GCC run: ./setup.sh
# This will copy Propeller GCC to /opt/parallax and PropellerIDE
# to /opt/propelleride and create a shortcut for the executable
# /opt/propelleride/PropellerIDE in /usr/bin/propelleride 
#
# To start PropellerIDE use the command: propelleride
#
#---------------------------------------------------------------
#
# Copyright (c) 2014 by Parallax Inc.
# TERMS OF USE: MIT License (text at end of this file).
# 
#---------------------------------------------------------------


PROPELLERIDE_DIR=/opt/propelleride

# Show usage
usage()
{
	echo "Usage: setup.sh [action [target]]"
	echo "Install or remove PropellerIDE package."
	echo "Action:"
	echo "  help            Show this message"
	echo "  install         Install PropellerIDE"
	echo "  remove          Remove PropellerIDE"
	echo ""
}

# Remove PropellerIDE function
remove_propelleride()
{
	if [ -e $PROPELLERIDE_DIR ]; then
		read -p "Remove previously installed PropellerIDE? [y/N]:" response
		case $response in
			# Does the user really want to remove it?
			Y|y|YES|Yes|yes) echo "Removing PropellerIDE"
				rm -r $PROPELLERIDE_DIR
				rm /usr/bin/propelleride
			;;
			*) echo "Not removing PropellerIDE"
		esac
	else
		echo "PropellerIDE not installed"
	fi
}

# Install PropellerIDE function
install_propelleride()
{
	echo "Installing PropellerIDE"
	mkdir -p $PROPELLERIDE_DIR
	cp -R bin spin license $PROPELLERIDE_DIR
	cp $PROPELLERIDE_DIR/bin/propelleride.sh /usr/bin/propelleride

	#echo `ldd /opt/propelleride/bin/PropellerIDE`

	# do this if propelleride.sh file permissions don't carry.
	chmod 755 /usr/bin/propelleride
}

# Verify the user is root function
if [ $(whoami) != root ]; then
	echo This script must be run as root
	echo try: sudo ./setup.sh
	exit
fi

# Execution starts here
# Did the user provide any commands?
case $1 in
	# Install something
	install)
		install_propelleride
		exit
	;;

	# Remove something
	remove)
		remove_propelleride
		exit
	;;

	# Instructions unclear, show usage
	help)
		usage
		exit
	;;

	# Instructions unclear, show usage
	?*)
		echo Action \"$1\" not recognized
		usage
		exit
	;;
esac

# No instructions, install everything ...
install_propelleride

exit

#+---------------------------------------------------------------------
#| TERMS OF USE: MIT License
#+---------------------------------------------------------------------
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#+---------------------------------------------------------------------
#| End of terms of use License Agreement.
#+---------------------------------------------------------------------
#
