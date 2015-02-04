Introduction
============

OpenSpin is an open source compiler for the Spin/PASM language of the Parallax Propeller. It was ported from Chip Gracey's original x86 assembler code.

There is a solution/project files for VS 2008, and there are make files for use with GCC. Tested on MinGW, linux, and Mac OSX so far. It should work with any recent version of GCC. I've compiled with GCC 4.6 and 4.8.

They build the PropellerCompiler library and openspin.exe.

The code successfully compiles all of the Library files shipped with PropTool as well as all of the files available in the OBEX as of August 2012. That's approximately 1450 spin files.

The only binary differences are from the corrected handling of floating point numbers (which is now IEEE compliant).

As of version r41, it supports a basic [preprocessor](https://github.com/reltham/OpenSpin/wiki/Preprocessor).

PropellerCompiler library
-------------------------

PropellerCompiler.cpp/h contain the interface to the compiler. Look at openspin.cpp for an example of how to work with the interface.

openspin.exe
------------

openspin.exe is a command line wrapper for the compiler library. You give it a spin file and it passes it through the compiler and produces a .binary (or optional .eeprom) file with the same base name as the passed in spin file. There are several [command line options](https://github.com/reltham/OpenSpin/wiki/CommandLine) available. Run openspin.exe with no arguments to get a usage description.

Binary Downloads
----------------

[Binary build downloads are available.](https://github.com/reltham/OpenSpin/wiki/Downloads)

Thanks
------

* Thanks to Steve Denson, for the Makefile and testing on linux!
* Thanks to David Betz for testing on Mac OSX.
* Thanks to Eric Smith for providing the code and helping with integrating the preprocessor.
