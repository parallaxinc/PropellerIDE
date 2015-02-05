#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, re
import argparse
import subprocess
import xml.etree.ElementTree as etree

def get_version(version):
    oldpath = os.getcwd()
    if os.path.exists(version.attrib['path']):
        os.chdir(version.attrib['path'])

    process = subprocess.Popen(['git','describe','--tags','--long'],stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()

    a = re.findall("No names found", err)
    if a:
        return "0.0.0-phony"
    else:
        return out.split('-')[0]

    os.chdir(oldpath)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='plist.py - generate plist file from repo config')
    parser.add_argument('project', nargs=1, metavar="PROJECT", help="repo.xml file to parse")
    args = parser.parse_args()

    tree = etree.parse(args.project[0])
    root = tree.getroot()
    version = root.find('version')

    print get_version(version)
