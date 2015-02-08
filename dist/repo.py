#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, re
import argparse
import subprocess
import xml.etree.ElementTree as etree
import logging

class Repo:
    def __init__(self, repofile):
        self.name = repofile
        self.root = etree.parse(self.name).getroot()

    def run_git(self, gitargs, repo):
        oldpath = os.getcwd()
        if os.path.exists(repo.attrib['path']):
            os.chdir(repo.attrib['path'])
    
        if gitargs[0] == 'clone':
            os.chdir(oldpath)
    
        gitargs.insert(0,"git")
        process = subprocess.Popen(gitargs, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = process.communicate()

        logging.debug([gitargs, out, err])
        os.chdir(oldpath)

        return out, err

    def gfx(self):
        gfx = self.root.find('gfx')
        if gfx == None:
            raise Exception("Graphics path not found!")

        return gfx.attrib['path']

    def info(self):
        return self.root.find('info')

    def node(self, node):
        return self.root.find(node)

    def version(self):
        version = self.root.find('version')

        out, err = self.run_git(['describe','--tags','--long'], version)
    
        if out == '':
            return "0.0.0"
        else:
            return out.split('-')[0]

    def git_clone(self, repo):
        gitargs = ['clone']
        gitargs.append(repo.attrib['url'])
        gitargs.append(repo.attrib['path'])
        self.run_git(gitargs, repo)
    
    def git_pull(self, repo):
        gitargs = ['pull']
        self.run_git(gitargs, repo)

    def git_update(self, repo):
        if os.path.exists(repo.attrib['path']):
            if repo.attrib['path'] == '.':
                self.git_clone(repo)
            else:
                self.git_pull(repo)
        else:
            self.git_clone(repo)

        self.run_git(['submodule','init'], repo)
        self.run_git(['submodule','update'], repo)


    def git_switch(self, repo):
        if 'branch' in repo.attrib:
            self.run_git(['checkout',repo.attrib['branch']], repo)
        elif 'tag' in repo.attrib:
            self.run_git(['checkout',repo.attrib['tag']], repo)
        else:
            self.run_git(['checkout','master'], repo)


    def validate_repo(self, repo):
        if not 'path' in repo.attrib:
            raise KeyError("No path specified in repo")
    
        if not 'url' in repo.attrib:
            raise KeyError("No URL specified in repo")

        print repo.attrib['path'], repo.attrib['url']

    def git_checkout(self, repo):
        print "--- "+repo.attrib['path']+" ---"
        self.validate_repo(repo)
        self.git_update(repo)
        self.git_switch(repo)

    def build_tree(self):
        for child in self.root.findall('repo'):
            self.git_checkout(child)

    def build_filelist(self):
        output = ""
        for child in self.root.findall('repo'):
            self.validate_repo(child)
            out, err = self.run_git(['ls-files'], child)
            output += out
        return output
        

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='repo.py - make checking out your Git project more complicated\n\nNo parameters builds the project repository')
    parser.add_argument('project', nargs=1, metavar="PROJECT", help="Project XML config file to check out")
    parser.add_argument('-l','--log', nargs=1, metavar="LEVEL", help="Log level of debug output (DEBUG, INFO, WARNING, ERROR, CRITICAL)")
    parser.add_argument('-v','--version', action='store_true', help="Get the project version")
    parser.add_argument('-g','--gfx', action='store_true', help="Get the graphics path")
    parser.add_argument('--list-files', action='store_true', help="List all files in super-repository")

    args = parser.parse_args()

    if args.log:
        logging.basicConfig(level=args.log[0])

    repo = Repo(args.project[0])

    if args.version:
        print repo.version()
    elif args.gfx:
        print repo.gfx()
    elif args.list_files:
        print repo.build_filelist()
    else:
        repo.build_tree()

