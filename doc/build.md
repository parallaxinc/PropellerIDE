# Building PropellerIDE

The following dependencies are needed to build PropellerIDE:

 * Qt5.3 or later

PropellerIDE has been built on the following platforms:

 * Windows (Vista, 7, 8)
 * Mac OS X (10.6 onward)
 * Ubuntu (12.04 onward)
 * Raspbian OS
 * pcDuino

PropellerIDE has been built with the following compiler toolchains:

 * GCC
 * MinGW-x64
 * Clang
 * MSVC

## Getting the source

Check out the project and its dependencies.

```
git clone https://github.com/parallaxinc/PropellerIDE.git
git submodule update --init --recursive
```

## Building the executable

### Using `qmake`

PropellerIDE can be built from the command-line using `qmake` to generate makefiles.

```
cd PropellerIDE
qmake
```

If you have made changes to the `.pro` files, remember to use `-r` to update all makefiles, not just the current one.

```
qmake -r
```

Use Make to build the project. On most Linuxes, GNU Make is ubiquitous. PropellerIDE supports parallel builds, so make sure to specify the number of jobs with `-j`

```
make -j16
```


#### on Windows

You will need to download Qt5 from the Qt website. You will also need Inno Setup to build the Windows installer.

* https://www.qt.io/download/
* http://www.jrsoftware.org/isinfo.php

Qt is distributed with either a MinGW or MSVC toolchain on Windows. Be sure to add the paths to the toolchain and Inno Setup to the system environment.

```
C:\Qt\Tools\mingw482_32\bin;C:\Qt\5.3\mingw482_32\bin;C:\Program Files (x86)\Inno Setup 5
```

The MinGW toolchain is painfully slow, but you can build with `mingw32-make`, which supposedly supports parallel builds but is slow as a dog anyway so it makes little difference.

```
mingw32-make
```

If you've decided to install Visual Studio, you'll have an instance of `nmake`. You can enable parallel builds by setting the CL environment variable, which will speed things up considerably.

```
set CL=/MP
```

#### all platforms

The makefiles support standard makefile targets: `make clean` removes object files, `make distclean` removes object files and makefiles.

### Using QtCreator

PropellerIDE may also be built with QtCreator, but it should be noted that QtCreator and `qmake` builds seem to be incompatible with each other, so `make distclean` should be called before switching between them.

### Using CMake

Instructions on CMake builds are not yet available.

## Packaging For release

PropellerIDE is distributed in standalone packages using `packthing`, an open source packaging tool available on [GitHub](https://github.com/lamestation/packthing) or downloadable via the Python Package Index.

Via GitHub:

```
git clone https://github.com/lamestation/packthing
cd packthing
python setup.py install
```

Via PyPI:

```
pip install packthing
```
