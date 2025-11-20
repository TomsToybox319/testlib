#!/bin/bash
set -e

mkdir -p Build
cd Build

CompilerOptions="-o main"  # Output file will be named "main"
CompilerOptions+=" -Wall -Wextra -Werror"  # Turn on more compiler warnings 
                                           # and treat them as errors
CompilerOptions+=" -std=c++23"   # Compile with C++23 standard                                           
CompilerOptions+=" -I../Src/
clang++  ../Test/test_utest.cpp ../Src/unittest.cpp $CompilerOptions

cd ..
