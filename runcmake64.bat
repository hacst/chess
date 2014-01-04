@echo OFF
if not exist bin64 mkdir bin64
cd bin64
SET BOOST_ROOT=3rdparty/boost-build64/
cmake -G "Visual Studio 12 Win64" ..
cd ..