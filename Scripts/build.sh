#!/bin/bash
set -e

mkdir -p Build
cd Build

CompilerOptions="-o main"  # Output file will be named "main"
CompilerOptions+=" -Wall -Wextra -Werror"  # Turn on more compiler warnings 
                                           # and treat them as errors
clang++ ../Src/main.cpp $CompilerOptions

cd ..
