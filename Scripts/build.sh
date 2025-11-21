#!/bin/bash
set -e

mkdir -p Build
cd Build

CompilerOptions="-o main"  # Output file will be named "main"
CompilerOptions+=" -Wall  -Werror "  # Turn on more compiler warnings and treat them as errors
CompilerOptions+=" -Weverything" # Warn all of the things!
CompilerOptions+=" -Wno-c++98-compat" # Don't need C++98 comptability
CompilerOptions+=" -Wno-padded" # Don't care about padding
CompilerOptions+=" -Wno-global-constructors" # We need global ctors
CompilerOptions+=" -Wno-exit-time-destructors" # We need exit-time dtors
CompilerOptions+=" -std=c++23"   # Compile with C++23 standard                                           
CompilerOptions+=" -I../Src/"    # Set include directory
clang++  ../Test/test_utest.cpp ../Src/unittest.cpp $CompilerOptions

cd ..
