#!/usr/bin/env python
# -*- coding: utf-8 -*-

import plistlib
import argparse
import xml.etree.ElementTree as etree

parser = argparse.ArgumentParser(description='plist.py - generate plist file from repo config')
parser.add_argument('project', nargs=1, metavar="PROJECT", help="repo.xml file to parse")
parser.add_argument('-o','--out', nargs=1, default="Info.plist", metavar="OUTPUT", help="Output plist file (default: 'Info.plist')")
args = parser.parse_args()

tree = etree.parse(args.project[0])
root = tree.getroot()
info = root.find('info')
target = root.find('target/mac')

ORGANIZATION = info.attrib['organization']
NAME = info.attrib['application']
YEAR = info.attrib['copyright']
LICENSE = info.attrib['license']

CATEGORY = target.attrib['category']

pl = dict(
	CFBundleDevelopmentRegion = "English",
	CFBundleDisplayName = NAME,
	CFBundleExecutable = NAME,
	CFBundleIconFile = "propelleride.icns",
    CFBundleIdentifier = "com.parallax."+NAME,
	CFBundleInfoDictionaryVersion = "6.0",
	CFBundleName = NAME,
	CFBundlePackageType = "APPL",
	CFBundleShortVersionString = "0.20",
	CFBundleVersion = "1",
	LSApplicationCategoryType = CATEGORY,
	LSMinimumSystemVersion = "10.7",
	NSHumanReadableCopyright = u"Copyright © "+YEAR+", "+ORGANIZATION+". "
                                +NAME+" is released under the "+LICENSE+" license.",
	NSPrincipalClass = "NSApplication",
	NSSupportsSuddenTermination = "YES",
)

plistlib.writePlist(pl, args.out)
