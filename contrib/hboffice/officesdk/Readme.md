# OfficeSDK (LibreOffice-SDK C wrapper) for hboffice developers

* [Introduction](#introduction)
* [Installation of LibreOffice-SDK](#installation-of-libreoffice-sdk)
    - [Installation in Windows](#installation-in-windows)
    - [Installation in Linux](#installation-in-linux)
    - [Installation in macOS](#installation-in-macos)
* [Installation of CMake](#installation-of-cmake)
* [Build officesdk](#build-officesdk)
    - [Build officesdk in Windows](#build-officesdk-in-windows)
    - [Build officesdk in Linux](#build-officesdk-in-linux)
    - [Build officesdk in macOS](#build-officesdk-in-macos)
* [Vendoring the result](#vendoring-the-result)

## Introduction

**officesdk** is the C++ wrapper around the LibreOffice-SDK (UNO) that exposes a flat, `extern "C"` API (`include/officesdk.h`, `include/sheetsdk.h`, `include/writersdk.h`) consumed by `hboffice`. It has no dependency on any other third-party library besides the LibreOffice-SDK itself.

This directory is **self-contained**: it has its own `CMakeLists.txt` and `build.bat`/`build.sh`, independent from the rest of `hboffice` (which is built with `hbmk2`, not CMake). You only need to touch anything here if you're modifying `officesdk.cpp`/`helpers.cpp` or need to refresh the vendored binary in `bin/<win|linux|darwin>` (e.g. after a LibreOffice-SDK upgrade). Regular `hboffice` users/consumers never need to build this.

> **Important:** The `officesdk.dll`/`.lib`/`.so`/`.dylib` produced here are committed to git under `bin/` so that consumers of `hboffice` don't need Visual Studio or the LibreOffice-SDK at all.

## Installation of LibreOffice-SDK

> **Important:** You also need the regular LibreOffice program installed (see the main [Readme.md](../Readme.md#installation-of-libreoffice)). The SDK alone is not enough to run/test anything.

### Installation in Windows

* Install the LibreOffice-SDK package.
    > **Important:** LibreOffice-SDK is available in 32-bit and 64-bit versions. You will need to compile `officesdk` in 32 or 64 bits depending on the version of LibreOffice installed. It is not possible to mix 32 and 64 bit binaries. **By default, in Windows, officesdk will be compiled in 64bit**.
    > **Important:** The SDK version must be the same as the LibreOffice application version.

* [LibreOffice Windows download](https://download.documentfoundation.org/libreoffice/stable/26.2.5/win/x86_64/LibreOffice_26.2.5_Win_x86-64.msi)

* [LibreOffice Windows SDK download](https://download.documentfoundation.org/libreoffice/stable/26.2.5/win/x86_64/LibreOffice_26.2.5_Win_x86-64_sdk.msi)

* Make sure `LIBREOFFICE_HOME` is set (see main Readme), it's also used to locate the SDK headers/libs (`%LIBREOFFICE_HOME%\sdk`).

### Installation in Linux

* Install the LibreOffice development libraries.
    ```
    sudo apt-get install libreoffice-dev

    # Optional, not mandatory for compile
    sudo apt-get install libreoffice-dev-doc
    ```
* Make sure `LIBREOFFICE_HOME` is set (see main Readme).

### Installation in macOS

* Download the LibreOffice-SDK package (`LibreOffice-SDK.dmg`). Move the extracted folder wherever you want, e.g. `/Applications/libreoffice-sdk`.

* Set the `LIBREOFFICE_SDK` environment variable with the path to the LibreOffice-SDK folder (`/Applications/libreoffice-sdk` in this example).
    ```
    nano .zshrc
    export LIBREOFFICE_SDK=/Applications/libreoffice-sdk
    source .zshrc
    ```

* Set the `DYLD_LIBRARY_PATH` environment variable to point the main package framework libraries.
    ```
    nano .zshrc
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LIBREOFFICE_HOME/Contents/Frameworks
    ```

* Go to `$LIBREOFFICE_HOME/Contents/Frameworks` and create the symbolic links for these libraries (the final `.3` version can differ in your installation):
    ```
    /Applications/LibreOffice.app/Contents/Frameworks
    ln -s libuno_cppu.dylib.3 libuno_cppu.dylib
    ln -s libuno_cppuhelpergcc3.dylib.3 libuno_cppuhelpergcc3.dylib
    ln -s libuno_purpenvhelpergcc3.dylib.3 libuno_purpenvhelpergcc3.dylib
    ln -s libuno_sal.dylib.3 libuno_sal.dylib
    ln -s libuno_salhelpergcc3.dylib.3 libuno_salhelpergcc3.dylib
    ```

## Installation of CMake

```
:~/$ cmake --version
cmake version 3.10.2
```

## Build officesdk

### Build officesdk in Windows

> **Important:** Visual Studio is imperative to build `officesdk.dll` (MinGW is not supported). LibreOffice-SDK does not distribute the source code of these libraries: `icppu`, `icppuhelper`, `ipurpenvhelper`, `isal`, `isalhelper`
located in `LIBREOFFICE_HOME/sdk/lib`. Only the already compiled .lib, generated with MSVC. The real problem is that they are C++ libraries, not a flat C API: they expose classes (Reference<>, UNO exceptions, etc.) whose name mangling of C++ symbols depends on the compiler's ABI. MSVC uses its own mangling scheme and its own exception model. MinGW and Clang-on-Windows use the Itanium ABI (the same as GCC on Linux). They are incompatible with each other: the MinGW/Clang linker looks for symbols with a mangling that simply does not exist in the .lib compiled by MSVC.

```
cd contrib\hboffice\officesdk
set CMAKE_GENERATOR=Visual Studio 18 2026
build.bat -b Release
```

`officesdk.dll`, `officesdk.lib` and `officesdk.exp` will be generated directly in `bin\win`.

If you have a different Visual Studio version installed, change `CMAKE_GENERATOR` accordingly (`Visual Studio 17 2022`, `Visual Studio 16 2019`, etc. at least VS 2012, since the LibreOffice-SDK requires C++11).

### Build officesdk in Linux

```
cd contrib/hboffice/officesdk
./build.sh -comp gcc -b Release
```

`libofficesdk.so` will be generated directly in `bin/linux`.

### Build officesdk in macOS

> **Important:** Xcode is required to build in macOS. Use the same `MACOSX_DEPLOYMENT_TARGET` as when building Harbour.

```
cd contrib/hboffice/officesdk
export MACOSX_DEPLOYMENT_TARGET=13.0
./build.sh -b Release
```

`libofficesdk.dylib` will be generated directly in `bin/darwin`.

## Vendoring the result

Once the build succeeds, the binaries already sit in the vendored location (`bin/<win|linux|darwin>`). There's no separate copy step. Just `git add/commit` them like any other tracked file:

```
git add bin/win        # or bin/linux, bin/darwin
git commit -m "Refresh vendored officesdk.dll (LibreOffice-SDK X.Y)"
```

> **Important:** Only the Release build is meant to be vendored/committed. It's the one distributed to `hboffice` consumers.
