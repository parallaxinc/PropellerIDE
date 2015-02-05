#!/usr/bin/env python
# -*- coding: utf-8 -*-

import string

import argparse
import xml.etree.ElementTree as etree

parser = argparse.ArgumentParser(description='iss.py - generate Inno Setup install script from template')
parser.add_argument('project', nargs=1, metavar="PROJECT", help="repo.xml file to parse")
parser.add_argument('-o','--out', nargs=1, default="installer.iss", metavar="OUTPUT", help="Output .iss file (default: 'installer.iss')")
args = parser.parse_args()

script = string.Template("""
[Setup]

AppID=${APPID}
AppName=${NAME}
AppVersion=${VERSION}
AppPublisher=${ORGANIZATION}
AppPublisherURL=${WEBSITE}
AppSupportURL=${WEBSITE}
AppUpdatesURL=${WEBSITE}
DefaultDirName={pf}\\${NAME}
DefaultGroupName=${NAME}
OutputDir="../staging/"
OutputBaseFilename=${SHORTNAME}-${VERSION}-win-setup
Compression=lzma/Max
SolidCompression=true
AlwaysShowDirOnReadyPage=true
UserInfoPage=no
UsePreviousUserInfo=no
DisableDirPage=yes
DisableProgramGroupPage=yes
DisableReadyPage=no
WizardImageFile="..\gfx\propellerbanner.bmp"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "../staging/${SHORTNAME}/*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\\${NAME}"; Filename: "{app}\\${SHORTNAME}.exe" ; IconFilename: "{app}\propellerhat.ico";
Name: "{group}\{cm:UninstallProgram,{NAME}}"; Filename: "{uninstallexe}";
Name: "{commondesktop}\\${NAME}"; Filename: "{app}\\${SHORTNAME}.exe"; IconFilename: "{app}\propellerhat.ico";
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\\${NAME}"; Filename: "{app}\\${SHORTNAME}.exe"; IconFilename: "{app}\propellerhat.ico";

[Run]
Filename: {app}\\${SHORTNAME}.exe; Description: "{cm:LaunchProgram,${NAME}}"; Flags: skipifsilent NoWait PostInstall; 
""")

tree = etree.parse(args.project[0])
root = tree.getroot()
target = root.find('target/win')
info = root.find('info')

f = open(args.out, 'w')
f.write(script.substitute(
            APPID           = target.attrib['AppID'],
            ORGANIZATION    = info.attrib['organization'],
            NAME            = info.attrib['application'],
            SHORTNAME       = info.attrib['application'].lower(),
            WEBSITE         = info.attrib['url'],
            VERSION         = 'DEADBEFF'
        )
    )

