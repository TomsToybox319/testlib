#!/bin/bash
set -e
set -x

mkdir -p Build
cd Build

CompilerOptions+=" -Wall  -Werror "  # Turn on more compiler warnings and treat them as errors
CompilerOptions+=" -Weverything" # Warn all of the things!
CompilerOptions+=" -Wno-c++98-compat" # Don't need C++98 comptability
CompilerOptions+=" -Wno-padded" # Don't care about padding
CompilerOptions+=" -Wno-global-constructors" # We need global ctors
CompilerOptions+=" -Wno-exit-time-destructors" # We need exit-time dtors
CompilerOptions+=" -Wno-weak-vtables" # Disable warnings about vtables
CompilerOptions+=" -std=c++23"   # Compile with C++23 standard                                           
CompilerOptions+=" -I../Src/"    # Set include directory

clang++ -c ../Src/unittest.cpp $CompilerOptions -o unittest.o
clang++ ../Test/test_utest.cpp unittest.o -o main $CompilerOptions

cd ..
