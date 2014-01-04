cd boost && bootstrap.bat
b2 address-model=32 toolset=msvc-12.0 --stagedir=stage32 --prefix=../boost-build --build-type=complete -j6 debug release install
b2 address-model=64 toolset=msvc-12.0 --stagedir=stage64 --prefix=../boost-build64 --build-type=complete -j6 debug release install