#
# Build all script
# Harbour/GTNAP/HBAWS/HBOFFICE
# Using different compilers (VS, MinGW-GCC, MinGW-Clang)
# Important! This script will remove previous Harbour compilations
# It can take several minutes to finish all tasks
#
# Options: -comp [gcc|clang]  (gcc default)
#          -b [Debug|Release] (Release default)
#          -noharbour (Avoid recompile Harbour)
#
# bash build_all.sh -comp gcc -b Release
# bash build_all.sh -comp gcc -b Release &> full_build_log.txt
# bash build_all.sh -comp clang -b Release &> full_build_log.txt
# bash build_all.sh -noharbour -comp gcc -b Release &> noharbour_build_log.txt
#
BUILD_HARBOUR=yes
ALL_BUILD_COMPILER=gcc
PLATFORM=linux
BUILD=Release
HBMK_FLAGS=

if [ "$(uname)" == "Darwin" ]; then
    ALL_BUILD_COMPILER=clang
    PLATFORM=darwin
fi

while [[ $# -gt 0 ]]; do
  case $1 in
    -comp)
      ALL_BUILD_COMPILER="$2"
      shift
      shift
      ;;
    -b)
      BUILD="$2"
      shift
      shift
      ;;
    -noharbour)
      BUILD_HARBOUR=no
      shift
      ;;
    -*|--*)
      shift
      ;;
  esac
done

cd ..

# Allowed compilers for Unix-like
if [[ "$ALL_BUILD_COMPILER" != "gcc" && "$ALL_BUILD_COMPILER" != "clang" ]]; then
    echo Error Unknown compiler: $ALL_BUILD_COMPILER
    exit 1
fi

echo ----------------------------------------
echo Beginning BUILD_ALL script
echo BUILD_HARBOUR=$BUILD_HARBOUR
echo ALL_BUILD_COMPILER=$ALL_BUILD_COMPILER
echo BUILD=$BUILD
echo ----------------------------------------

# Compile Harbour with gcc or clang
if [[ "$BUILD_HARBOUR" == "yes" ]]; then
    # Remove previous compilations
    rm -rf bin/$PLATFORM/$ALL_BUILD_COMPILER
    rm -rf lib/$PLATFORM/$ALL_BUILD_COMPILER
    make clean
    make -j4 HB_CPU=x86_64 HB_COMPILER=$ALL_BUILD_COMPILER
    echo ----------------------------------------
    echo Harbour $ALL_BUILD_COMPILER build successfully
    echo ----------------------------------------
fi

# Compile HBOffice
cd contrib/hboffice
rm -rf build

# First, the vendored officesdk.so/.dylib (only maintainers need to rebuild it)
cd officesdk
bash build.sh -comp $ALL_BUILD_COMPILER -b $BUILD || { echo "Error building HBOffice officesdk" ; exit 1; }
cd ..
echo --------------------------------
echo hboffice officesdk build successfully
echo --------------------------------

# Then, the hboffice lib
bash build.sh -comp $ALL_BUILD_COMPILER -b $BUILD || { echo "Error building HBOffice LIB" ; exit 1; }
echo --------------------------------
echo hboffice $ALL_BUILD_COMPILER lib build successfully
echo --------------------------------

# Return to harbour main path after hboffice
cd ..
cd ..

# Build hbaws
cd contrib/hbaws
rm -rf build
bash build.sh -comp $ALL_BUILD_COMPILER -b $BUILD || { echo "Error building HBAWS" ; exit 1; }
echo -------------------------
echo hbaws $ALL_BUILD_COMPILER build successfully
echo -------------------------

# Return to harbour main path after hbaws
cd ..
cd ..

# Build gtnap
cd contrib/gtnap
rm -rf build
bash build.sh -comp $ALL_BUILD_COMPILER -b $BUILD || { echo "Error building GTNAP" ; exit 1; }
echo -------------------------
echo gtnap $ALL_BUILD_COMPILER build successfully
echo -------------------------

cd tests/cuademo/gtnap_cualib
rm exemplo

if [ "$(uname)" == "Darwin" ]; then
    export PATH=$PATH:/Applications/LibreOffice.app/Contents/MacOS
    export DYLD_LIBRARY_PATH=$AWS_SDK_ROOT/$ALL_BUILD_COMPILER/Release/lib:$LIBREOFFICE_HOME/Contents/Frameworks:$HARBOUR_HOME/contrib/hboffice/officesdk/bin/$PLATFORM
else
    export LD_LIBRARY_PATH=$AWS_SDK_ROOT/$ALL_BUILD_COMPILER/Release/lib:$LIBREOFFICE_HOME/program:$HARBOUR_HOME/contrib/hboffice/officesdk/bin/$PLATFORM
fi

../../../../../bin/$PLATFORM/$ALL_BUILD_COMPILER/hbmk2 -comp=$ALL_BUILD_COMPILER exemplo.hbp
./exemplo --hb:gtnap

# Return to gtnap path
cd ..
cd ..
cd ..

# Return to harbour main path after hboffice
cd ..
cd ..

echo ---------------------------------------
echo All build jobs generated successfully
echo ---------------------------------------
echo ../contrib/gtnap/tests/cuademo/gtnap_cualib/exemplo --hb:gtnap
