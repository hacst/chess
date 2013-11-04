@echo OFF
if not exist bin mkdir bin
cd bin
SET BOOST_ROOT=3rdparty/boost-build/
cmake -G "Visual Studio 12" ..
cd ..