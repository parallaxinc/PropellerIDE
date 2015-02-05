#!/usr/bin/env python

import os, sys, re
import argparse
import subprocess
import xml.etree.ElementTree as etree

parser = argparse.ArgumentParser(description='repo.py - make checking out your Git project more complicated')
parser.add_argument('project', nargs=1, metavar="PROJECT", help="Project XML config file to check out")
args = parser.parse_args()

tree = etree.parse(args.project[0])
root = tree.getroot()

def run_git(gitargs):
    oldpath = os.getcwd()
    if os.path.exists(child.attrib['path']):
        os.chdir(child.attrib['path'])

    if gitargs[0] == 'clone':
        os.chdir(oldpath)

    gitargs.insert(0,"git")
    rc = subprocess.call(gitargs, stdout=open(os.devnull,'w'))

    os.chdir(oldpath)

    return rc

for child in root.findall('repo'):

    if not 'name' in child.attrib:
        raise KeyError("Repository not named")

    if not 'path' in child.attrib:
        raise KeyError("No path specified in repo")

    if not 'url' in child.attrib:
        raise KeyError("No URL specified in repo")

    print "Checking out "+child.attrib['name']+" to "+child.attrib['path']+"..."

    if os.path.exists(child.attrib['path']):
        if child.attrib['path'] == '.':
            gitargs = ['clone']
            gitargs.append(child.attrib['url'])
            gitargs.append(child.attrib['path'])
        else:
            gitargs = ['pull']
    else:
        gitargs = ['clone']
        gitargs.append(child.attrib['url'])
        gitargs.append(child.attrib['path'])

    run_git(gitargs)

    if 'branch' in child.attrib:
        run_git(['checkout',child.attrib['branch']])

    if 'tag' in child.attrib:
        run_git(['checkout',child.attrib['tag']])

