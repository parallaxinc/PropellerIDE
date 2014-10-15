PropellerIDE
============

Fork of the open source PropellerIDE

### Checkout

```
git clone https://github.com/lamestation/PropellerIDE.git PropellerIDE
cd PropellerIDE
git submodule init
git submodule update
```

### Setting Up The Build Environment

```
sudo apt-get install qtcreator qt5-default devscripts zlib1g-dev 
```

### Running


```
./plinrelease.sh
```

Then, to create a Debian package:

```
cd linux
./pack.sh ../propelleride-[whatever version you're on]
```
