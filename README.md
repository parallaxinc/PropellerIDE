PropellerIDE
============

![the interface](gfx/interface.png "The PropellerIDE interface")

PropellerIDE is an easy-to-use, cross-platform development tool for the Parallax Propeller microcontroller.

Write Spin code, download programs to your Propeller board, and debug your applications with the built-in serial terminal.

### Features

- Spin syntax highlighting, auto-complete, code suggestion, and auto-indent
- Unicode editing support
- Auto-detection of connected Propeller hardware
- Collapsible sidebar with function and program views
- Integrated serial terminal

### Planned Features

- Unified tree view showing hierarchy of objects and functinos
- Integrated Spin and Assembly help manual
- Plugin interface for extending the application

PropellerIDE was created using C++ and Qt.

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

## Credits

Originally developed by Steve Denson, Dennis Gately, and Roy Eltham.

**Related Projects**: [OpenSpin](https://github.com/parallaxinc/OpenSpin)
