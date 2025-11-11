#!/bin/bash
set -e

mkdir -p Build
cd Build

CompilerOptions="-o main"
CompilerOptions+=" -Wall -Wextra -Werror"
clang++ ../Src/main.cpp $CompilerOptions

cd ..
