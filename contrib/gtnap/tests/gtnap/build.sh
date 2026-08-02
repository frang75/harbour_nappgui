#!/bin/bash

#
# GTNAP demo build script
#
# Will generate the gtnap demo executable.
# build -comp [gcc|clang] -b [Debug|Release]

#
# Input parameters
#
PLATFORM=linux
COMPILER=gcc
BUILD=Release
CWD=$(pwd)

if [ "$(uname)" == "Darwin" ]; then
    PLATFORM=darwin
    COMPILER=clang
fi

while [[ $# -gt 0 ]]; do
  case $1 in
    -comp)
      COMPILER="$2"
      shift
      shift
      ;;
    -b)
      BUILD="$2"
      shift
      shift
      ;;
    -*|--*)
      shift
      ;;
  esac
done

#
# Beginning
#
echo ---------------------------
echo Generating GTNAP demo
echo Main path: $CWD
echo Build type: $BUILD
echo PLATFORM: $PLATFORM
echo COMPILER: $COMPILER
echo ---------------------------

HBMK_PATH=../../../../bin/$PLATFORM/$COMPILER
HBMK_FLAGS=

if [ $BUILD == "Debug" ]; then
    HBMK_FLAGS=-debug
fi

$HBMK_PATH/hbmk2 $HBMK_FLAGS -comp=$COMPILER $CWD/gtnap.hbp || exit 1

echo ---------------------------
echo GTNAP demo build succeed
echo ---------------------------

cd $CWD
