@echo OFF
if not exist bin mkdir bin
cd bin
ctest ../ -C Debug -V
cd ..