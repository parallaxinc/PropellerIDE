#!/bin/bash -x

set -e

# Inspired by 
# http://sentryytech.blogspot.com/2013/02/faster-compiling-on-emulated-raspberry.html
# http://www.unixmen.com/emulating-raspbian-using-qemu/


IMAGENAME=$1
#IMAGENAME=2015-05-05-raspbian-wheezy
IMAGE=${IMAGENAME}.img
ZIP=${IMAGENAME}.zip
MNT=~/rpi/mnt

HOME=`pwd`
RPIHOME=$MNT/home/pi

#mkdir -p ~/rpi
#pushd ~/rpi

# install dependencies

#sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid main restricted"
#sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid-updates main restricted"
#sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid universe"
#sudo add-apt-repository "deb http://us.archive.ubuntu.com/ubuntu/ vivid-updates universe"
#sudo apt-get update
#sudo apt-get install qemu-user-static qemu-kvm kpartx

# get and prepare Raspbian image

#wget -4 http://lamestation.net/downloads/travis/$ZIP
#unzip -o $ZIP
#qemu-img resize $IMAGE +3G

# mount image

#mkdir -p $MNT 
sudo kpartx -a -v $IMAGE
sudo mount /dev/mapper/loop0p2 $MNT

# set up arch chroot

mkdir -p $MNT/usr/bin
sudo cp -f ./inside-chroot.sh $MNT/
sudo cp /usr/bin/qemu-arm-static $MNT/usr/bin/
sudo mount -o bind /dev  $MNT/dev
sudo mount -o bind /proc $MNT/proc
sudo mount -o bind /sys  $MNT/sys
sudo mount --bind /home  $MNT/home
sudo cp -f /etc/network/interfaces $MNT/etc/network/interfaces
sudo cp -f /etc/resolv.conf $MNT/etc/resolv.conf 
#sudo sed -e's/^/#/g' -i $MNT/etc/ld.so.preload
#sudo echo 'deb http://mirrordirector.raspbian.org/raspbian/ jessie main contrib non-free rpi' >> $MNT/etc/apt/sources.list

# install qemu user mode emulation into kernel

sudo su -c ./register-interpreter.sh root
#sudo chroot $MNT/ /bin/bash -c "./inside-chroot.sh"

# run packthing checkout

# packthing rpi --checkout-only

# move project into chroot

#cd ~
#mv $TRAVIS_REPO_SLUG $MNT
#
#sudo chroot $MNT/ /bin/bash -c "./inside-chroot-script.sh"
#
#mv $MNT $TRAVIS_REPO_SLUG
