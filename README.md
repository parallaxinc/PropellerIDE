PropellerIDE
============

The new home of the Propeller IDE.

## Checkout

```
git clone https://github.com/lamestation/PropellerIDE.git PropellerIDE
cd PropellerIDE
make checkout
```

## The Build Environment

### Linux

#### Debian Packages
```
sudo apt-get install qtcreator qt5-default devscripts zlib1g-dev 
```

Type `make deb` in the project root to build a Debian package.

```
make deb
```

#### Fedora Packages

*Not yet available.*

#### Linux Tarball

*Not yet available.*

#### Build for 32-bit on a 64-bit platform

```
sudo apt-get install gcc-multilib libc6-i386 libc6-dev-i386
```

### Windows

#### Cross-Compiling From Linux

*Not yet available.*

The following dependencies are needed if you are planning to cross-compile to Windows.

```
sudo apt-get install mingw32 nsis
```

Type `make win` to execute the build.

```
make win
```

#### With Cygwin

*Not yet available.*

### Mac OS X

*Not yet available.*

#### Cross-Compiling From Linux

*Not yet available.*

#### Building On 


## Clean Up

Type `make clean` to remove old build files.

```
make clean
```
