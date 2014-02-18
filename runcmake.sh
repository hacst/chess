#!/bin/sh
mkdir -p bin
cd bin && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=`which gcc-4.8` -DCMAKE_CXX_COMPILER=`which g++-4.8` ..
cd ..
