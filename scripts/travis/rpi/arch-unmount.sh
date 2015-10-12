#!/bin/bash -x

IMAGE=${1}
MNT=mnt

sudo umount $MNT/dev
sudo umount $MNT/proc
sudo umount $MNT/sys
sudo umount $MNT/home
sudo umount $MNT/

sudo kpartx -d -v $IMAGE
rmdir $MNT
