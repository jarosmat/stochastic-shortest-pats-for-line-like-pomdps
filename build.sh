#!/bin/sh

cd "src" || exit
cmake -B build
cmake --build build

export PYTHONPATH="$PWD/build:\$PYTHONPATH"

cd ..
