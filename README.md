#elf-runpath#
===========

This tool allows the modification of [RPATH](http://en.wikipedia.org/wiki/Rpath)  in libraries or executables in ELF format. I wrote the elf tool mainly to set RPATH to $ORIGIN in Unix dynamic libraries or shared objects. The reason to do so is to make sure the correct load of libraries which in turn also depends on other libraries without rely on any external configuration, such as the modification of LD_LIBRARY_PATH. The $ORIGIN tells the dynamic liker to load the dependencies from the same directory from where the parent object was loaded.

