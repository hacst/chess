@echo OFF
if not exist bin mkdir bin
cd bin
cmake -G "Visual Studio 12" ..
cd ..