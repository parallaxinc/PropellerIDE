# Installing PropellerIDE

PropellerIDE is currently officially supported on **Windows**, **Mac**, **Debian**, and **Raspbian OS**.

## Windows

PropellerIDE is packaged as a Windows installer that will guide the user throughout the installation process.

## Mac

PropellerIDE is packaged as a regular DMG image, so mount the Volume and drag the icon into the Applications folder.

## Debian

PropellerIDE requires a minimum of Qt 5.3 which is only available on Ubuntu as of
14.10.

After downloading the Debian package for your platform, install it with `dpkg`.

```
sudo dpkg -i propelleride-(version)-amd64.deb
```

It will complain about dependencies at which point you can run `apt-get` to fix them.

```
sudo apt-get install -f
```

Make sure you install the FTDI drivers!

```
sudo apt-get install libftdi1
```

### Ubuntu 14.04 or earlier

PropellerIDE is known to build in Ubuntu versions as old as 12.04, but doing so will take some work.

Add the Utopic Unicorn sources to your `/etc/apt/sources.list`.

```
deb http://cz.archive.ubuntu.com/ubuntu utopic main
```

Run an update to ensure your apt repositories are up-to-date.

```
sudo apt-get update
```

### Raspberry Pi - Raspbian Wheezy

Qt5 is not available in the standard repository, but you can obtain it from Debian backports.

Add the following entries to `/etc/apt/sources.list`.

```
deb http://twolife.be/raspbian/ wheezy main backports
deb-src http://twolife.be/raspbian/ wheezy main backports
```

Add the repository key.

```
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-key 2578B775
```

Update and install Qt5 and its dependencies.

```
sudo apt-get update
sudo apt-get install qt5-default qt5-qmake libegl1-mesa libgles2-mesa
```
