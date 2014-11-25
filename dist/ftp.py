#!/usr/bin/env python

import ftplib
import os, sys
import getpass, platform

filename = sys.argv[1]

print "Enter host:"
host = raw_input()
print "Enter user name:"
username = raw_input()
print "Enter password:"
passwd = getpass.getpass()

def upload(ftp, file):
    ext = os.path.splitext(file)[1]
    if ext in (".txt", ".htm", ".html"):
        ftp.storlines("STOR " + file, open(file))
    else:
        ftp.storbinary("STOR " + file, open(file, "rb"), 1024)

ftp = ftplib.FTP(host)
ftp.login(username,passwd)
ftp.cwd("/public_html/downloads/"+platform.system())

upload(ftp, filename)
