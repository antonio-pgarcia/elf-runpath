#elf-runpath#

This tool allows the modification of [RPATH](http://en.wikipedia.org/wiki/Rpath)  in libraries or executables in ELF format. The elf-rpath was developed aiming to set RPATH to $ORIGIN in Unix dynamic libraries or shared objects. The reason to modify the value of RPATH is to make sure the correct load of libraries which in turn also depends on other libraries without rely on any external configuration, such as the modification of LD_LIBRARY_PATH. The $ORIGIN tells the dynamic liker to load the dependencies from the same directory from where the parent object was loaded. 

##Building##

**```gcc elf_rpath.c -o elf_rpath -lelf```**

##Command line options##


Command line                                            | Description
------------------------------------------------------- | -------------
**elf_rpath –v** *```<file name>```*                    | Show the current value of RPATH header
**elf_rpath –o** *```<file name>```*                    | Sets the  RPATH header value to $ORIGIN
**elf_rpath** *```<file name>```* *```<rpath value>```* | Sets the  RPATH header to any user provided value

