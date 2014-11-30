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

ftp = ftplib.FTP(host)
ftp.login(username,passwd)
ftp.cwd("/public_html/downloads/"+platform.system())

command = 'STOR ' + filename
ftp.storbinary(command, open(filename, "rb"), 1024)
