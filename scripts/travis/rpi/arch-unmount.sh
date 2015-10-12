#!/bin/bash -x

IMAGENAME=$1
#IMAGENAME=2015-05-05-raspbian-wheezy
IMAGE=${IMAGENAME}.img
ZIP=${IMAGENAME}.zip
MNT=mnt


sudo umount $MNT/dev
sudo umount $MNT/proc
sudo umount $MNT/sys
sudo umount $MNT/home
sudo umount $MNT/

sudo kpartx -d -v $IMAGE
