#!/bin/bash

#
# HBAWS Harbour examples build script
#
# Compiles all the example .prg tests and sets LD_LIBRARY_PATH/DYLD_LIBRARY_PATH
# to the AWS-SDK libraries required by the resulting executables.
#
# test -comp [gcc|clang]
#

COMPILER=gcc
PLATFORM=linux
CWD=$(pwd)

if [ "$(uname)" == "Darwin" ]; then
    COMPILER=clang
    PLATFORM=darwin
fi

while [[ $# -gt 0 ]]; do
  case $1 in
    -comp)
      COMPILER="$2"
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
echo Compiling HBAWS Harbour examples
echo COMPILER: $COMPILER
echo ---------------------------

HBMK_PATH=../../../../bin/$PLATFORM/$COMPILER

if [ "$PLATFORM" == "linux" ]; then
    # Vendored AWS-SDK binaries (awssdk/bin/linux/gcc). Linux gcc and clang
    # share the same GNU/Itanium C++ ABI, so one build serves both.
    export LD_LIBRARY_PATH=$CWD/../../awssdk/bin/linux/gcc:$LD_LIBRARY_PATH
else
    # macOS: not vendored yet, relies on a user-built AWS-SDK via AWS_SDK_ROOT.
    export DYLD_LIBRARY_PATH=$AWS_SDK_ROOT/$COMPILER/Release/lib:$DYLD_LIBRARY_PATH
fi

for P in listall listpage upload uploadm copy copym download delete restore; do
    echo Compiling $P.prg...
    $HBMK_PATH/hbmk2 $P.prg credentials.prg hbaws.hbc -comp=$COMPILER || exit 1
done

echo ------------------------
echo HBAWS examples build succeed
echo ------------------------
