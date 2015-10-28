#!/bin/bash
echo "Building Checker~~~~~~~~~~~~~~~~~~~~~~~~"
cp mylib/lib* ../../../Release+Asserts/lib/
make
mkdir Bin
g++ verify.cpp -o Bin/Checker
echo "Building finished!-----------------------Start run program in Directory Bin"

