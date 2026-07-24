#!/bin/bash

#
# OfficeSDK build script
#
# Maintainer-only: (re)generates the vendored libofficesdk.so/.dylib
# (LibreOffice-SDK C-Wrapper) into bin/<platform>.
# End users consuming hboffice don't need to run this script.
#
# build -comp [gcc|clang] -b [Debug|Release]

#
# Input parameters
#
COMPILER=gcc
BUILD=Release
CWD=$(pwd)

if [ "$(uname)" == "Darwin" ]; then
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
echo Generating OfficeSDK
echo Main path: $CWD
echo Build type: $BUILD
echo COMPILER: $COMPILER
echo ---------------------------

if [ "$(uname)" == "Darwin" ]; then
    cmake -G Xcode -S . -B build || exit 1
    cmake --build build --config $BUILD || exit 1
else
    mkdir -p build
    cd build
    if [ "$COMPILER" == "gcc" ]; then
        cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=$BUILD || exit 1
    fi

    if [ "$COMPILER" == "clang" ]; then
        cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang -DCMAKE_BUILD_TYPE=$BUILD || exit 1
    fi

    make -j 4 || exit 1
fi

echo ---------------------------
echo OFFICESDK DLL build succeed
echo ---------------------------

cd $CWD
