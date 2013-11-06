#!/bin/sh
mkdir -p bin
cd bin && cmake -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_CXX_COMPILER=g++-4.8 ..
cd ..
