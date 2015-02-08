#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, re
import argparse
import subprocess
import xml.etree.ElementTree as etree

parser = argparse.ArgumentParser(description='repo.py - make checking out your Git project more complicated')
parser.add_argument('project', nargs=1, metavar="PROJECT", help="Project XML config file to check out")
args = parser.parse_args()

tree = etree.parse(args.project[0])
root = tree.getroot()

def run_git(gitargs, repo):
    oldpath = os.getcwd()
    if os.path.exists(repo.attrib['path']):
        os.chdir(repo.attrib['path'])

    if gitargs[0] == 'clone':
        os.chdir(oldpath)

    gitargs.insert(0,"git")
    rc = subprocess.call(gitargs, stdout=open(os.devnull,'w'))

    os.chdir(oldpath)

    return rc

def git_clone(repo):
    gitargs = ['clone']
    gitargs.append(repo.attrib['url'])
    gitargs.append(repo.attrib['path'])
    run_git(gitargs, repo)

def git_pull(repo):
    gitargs = ['pull']
    run_git(gitargs, repo)


for child in root.findall('repo'):

    if not 'path' in child.attrib:
        raise KeyError("No path specified in repo")

    if not 'url' in child.attrib:
        raise KeyError("No URL specified in repo")

    print "--- "+child.attrib['path']+" ---"

    if os.path.exists(child.attrib['path']):
        if child.attrib['path'] == '.':
            git_clone(child)
        else:
            git_pull(child)
    else:
        git_clone(child)

    run_git(['submodule','init'], child)
    run_git(['submodule','update'], child)

    if 'branch' in child.attrib:
        run_git(['checkout',child.attrib['branch']], child)
    elif 'tag' in child.attrib:
        run_git(['checkout',child.attrib['tag']], child)
    else:
        run_git(['checkout','master'], child)

