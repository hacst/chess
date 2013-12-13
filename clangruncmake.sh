#!/bin/sh
mkdir -p clang-bin
cd clang-bin && cmake -DCMAKE_C_COMPILER=`which clang` -DCMAKE_CXX_COMPILER=`which clang++` ..
cd ..
