#!/usr/bin/env python
# -*- coding: utf-8 -*-

import plistlib
import argparse
from repo import Repo
import os

def build_plist(info, target):
    ORGANIZATION = info.attrib['organization']
    NAME = info.attrib['application']
    YEAR = info.attrib['copyright']
    LICENSE = info.attrib['license']
    CATEGORY = target.attrib['category']
    
    pl = dict(
    	CFBundleDevelopmentRegion = "English",
    	CFBundleDisplayName = NAME,
    	CFBundleExecutable = NAME,
    	CFBundleIconFile = "mac.icns",
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
    return pl

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='plist.py - generate plist file from repo config')
    parser.add_argument('project', nargs=1, metavar="PROJECT", help="repo.xml file to parse")
    parser.add_argument('-o','--out', nargs=1, metavar="OUTPUT", help="Output plist file (default: 'Info.plist')")
    parser.add_argument('-s','--show', action='store_true', help="Print output to terminal")
    args = parser.parse_args()

    repo = Repo(args.project[0])
    info = repo.info()
    target = repo.node('target/mac')

    pl = build_plist(info, target)

    if not args.out and not args.show:
        print "Command will not produce any output without -o or -s"

    if args.show:
        print pl

    if args.out:
        plistlib.writePlist(pl, args.out)
