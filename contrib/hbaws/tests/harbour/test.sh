#!/bin/bash

#
# HBAWS Harbour examples build script
#
# Compiles all the example .prg tests and sets LD_LIBRARY_PATH/DYLD_LIBRARY_PATH
# to the AWS-SDK libraries required by the resulting executables.
#
# test -comp [gcc|clang]
#
# Preserve LD_LIBRARY_PATH/DYLD_LIBRARY_PATH in the current shell.
# source test -comp [gcc|clang]
#

COMPILER=gcc
PLATFORM=linux
CWD=$(pwd)

if [ "$(uname)" = "Darwin" ]; then
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

if [ "$PLATFORM" = "linux" ]; then
    export LD_LIBRARY_PATH=$CWD/../../awssdk/bin/linux/gcc:$LD_LIBRARY_PATH
else
    export DYLD_LIBRARY_PATH=$CWD/../../awssdk/bin/$PLATFORM/clang:$DYLD_LIBRARY_PATH
fi

for P in listall listpage upload uploadm copy copym download delete restore; do
    echo Compiling $P.prg...
    # 'return' if this script is sourced (keeps the shell alive so the
    # LD_LIBRARY_PATH/DYLD_LIBRARY_PATH exported above stays set), 'exit'
    # if it's run as a normal subprocess.
    $HBMK_PATH/hbmk2 $P.prg credentials.prg hbaws.hbc -comp=$COMPILER || { return 1 2>/dev/null || exit 1; }
done

echo ------------------------
echo HBAWS examples build succeed
echo ------------------------
