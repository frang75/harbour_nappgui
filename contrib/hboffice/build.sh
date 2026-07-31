#!/bin/bash

#
# HBOffice build script
#
# Will generate the libhboffice.a with the Harbour wrapper. libofficesdk.so/
# .dylib is already vendored in officesdk/bin, so no LibreOffice-SDK is
# required for this step (see officesdk/Readme.md if you need to refresh the
# vendored library instead).
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
    export DYLD_LIBRARY_PATH=$LIBREOFFICE_HOME/Contents/Frameworks
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
echo Generating HBOFFICE
echo Main path: $CWD
echo Build type: $BUILD
echo PLATFORM: $PLATFORM
echo COMPILER: $COMPILER
echo ---------------------------

HBMK_PATH=../../bin/$PLATFORM/$COMPILER
HBMK_FLAGS=

if [ $BUILD == "Debug" ]; then
    HBMK_FLAGS=-debug
fi

$HBMK_PATH/hbmk2 $HBMK_FLAGS -comp=$COMPILER $CWD/hboffice.hbp || exit 1

echo ---------------------------
echo HBOFFICE LIB build succeed
echo ---------------------------

cd $CWD
