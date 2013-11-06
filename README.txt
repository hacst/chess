3D-Chess with artifical intelligence
====================================

Build requirements (Windows):
 - Visual Studio 2013
 - cmake 2.8.12 in path
 - Python 2.7 or later
 - Boost 1.55 (trunk)
 
Building on Windows:
 - Checkout boost SVN into 3rdparty directory
 - Run buildboost.bat in the same directory from a
   Visual Studio 2013 command prompt (this will take
   a while)
 - Run runcmake.bat in the root directory to generate project
   files.
 - Open .sln projectfile in build/ and build all

Build requirements (Linux):
 - gcc >= 4.8
 - pthreads
 - Python 2.7
 - Boost >= 1.54

Ubuntu < 13.10:
 GCC >=4.8 from PPA needed: e.g. ppa:ubuntu-toolchain-r/test

Ubuntu < 14.04:
 Boost >=1.54 from PPA needed: e.g. ppa:boost-latest/ppa

