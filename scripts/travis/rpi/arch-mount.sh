#!/bin/bash -x

IMAGE=${1}
MNT=mnt

# mount image

mkdir -p $MNT 
sudo kpartx -a -v $IMAGE

sudo mount /dev/mapper/loop0p2 $MNT
sudo mount -o bind /dev  $MNT/dev
sudo mount -o bind /proc $MNT/proc
sudo mount -o bind /sys  $MNT/sys
sudo mount --bind /home  $MNT/home

# set up emulation

mkdir -p $MNT/usr/bin

pushd scripts/travis/rpi

sudo cp -vf ./inside-chroot.sh $MNT/
sudo cp -vf /usr/bin/qemu-arm-static $MNT/usr/bin/
sudo cp -vf /etc/network/interfaces $MNT/etc/network/interfaces
sudo cp -vf /etc/resolv.conf $MNT/etc/resolv.conf 
sudo sed -e's/^/#/g' -i $MNT/etc/ld.so.preload
sudo echo 'deb http://mirrordirector.raspbian.org/raspbian/ jessie main contrib non-free rpi' >> $MNT/etc/apt/sources.list

sudo su -c ./register-interpreter.sh root

echo "Entering chroot"
pwd
ls -l $MNT/
tree
sudo chroot $MNT/ /bin/bash -c "./inside-chroot.sh"

popd
