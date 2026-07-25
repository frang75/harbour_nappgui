# Harbour LibreOffice wrapper (hboffice)

* [Introduction](#introduction)
* [Installation of LibreOffice](#installation-of-libreoffice)
    - [Installation in Windows](#installation-in-windows)
    - [Installation in Linux](#installation-in-linux)
    - [Installation in macOS](#installation-in-macos)
* [Build hboffice](#build-hboffice)
    - [Build hboffice in Windows](#build-hboffice-in-windows)
    - [Build hboffice in Linux](#build-hboffice-in-linux)
    - [Build hboffice in macOS](#build-hboffice-in-macos)
* [hboffice examples](#hboffice-examples)
* [hboffice API](./ReadmeAPI.md)
* [For hboffice developers](#for-hboffice-developers)

## Introduction

**hboffice** is a project to use the LibreOffice-SDK in Harbour projects. It is an incomplete API, since the LibreOffice SDK is very extensive. It provides high-level functions in C that hide the complexity of using the SDK directly in C++. This C API is easily portable to Harbour.

**hboffice** communicates with the LibreOffice program through `officesdk`, a small C wrapper around the LibreOffice-SDK (UNO). The `officesdk.dll`/`libofficesdk.so`/`libofficesdk.dylib` binary is vendored directly in the `officesdk/bin` folder, platform by platform, so end users don't need to install the LibreOffice-SDK or Visual Studio to build `hboffice.lib`.

* **officesdk.dll**/**libofficesdk.so**/**libofficesdk.dylib**: Dynamic library that contains the C API and the linkage to LibreOffice. Already vendored in `officesdk/bin/<win|linux|darwin>`.

* **officesdk.lib**: (Only in Windows) Static library with the .dll exported symbols. Already vendored alongside `officesdk.dll`.

* **hboffice.lib**/**libhboffice.a**: Static library that contains the Harbour wrapper. You can use any compiler (MSVC, MinGW, GCC, Clang, etc). This is the only thing `build.bat`/`build.sh` builds.

> **Important:** The same vendored `officesdk.dll`/`.lib` is used regardless of the compiler chosen for `hboffice.lib` (`msvc64`, `mingw64` or `clang`). `officesdk` exposes a flat C API, so there's no C++ ABI incompatibility between compilers here.

## Installation of LibreOffice

It is necessary to **correctly install the LibreOffice package** on the machine that will run a hboffice-based application (this is unrelated to building `hboffice.lib`, which doesn't need LibreOffice installed at all).

### Installation in Windows

* Install the LibreOffice package.
    ![download_libreoffice](https://github.com/frang75/harbour_nappgui/assets/42999199/c410187b-3f27-473e-b756-4dce9b91fecd)

* Set the `LIBREOFFICE_HOME` environment variable with the path to the LibreOffice home directory (usually `C:\Program Files\LibreOffice`). hboffice will connect to the LibreOffice program at runtime using this variable.

    ![envvar_libreoffice](https://github.com/frang75/harbour_nappgui/assets/42999199/3ad38b78-9214-4567-94b8-94dcf926848f)

* Add `%LIBREOFFICE_HOME%/program` path to `PATH` environment variable. In order to run any hboffice-based application, LibreOffice .DLLs must be accesible and located.

    ![path_envvar](https://github.com/frang75/harbour_nappgui/assets/42999199/d0215a5e-8569-4dca-a313-f765ada84080)

### Installation in Linux

* Install the LibreOffice package.
    ```
    sudo apt-get install libreoffice
    ```
* Set the `LIBREOFFICE_HOME` environment variable with the path to the LibreOffice home directory (usually `/usr/lib/libreoffice`). hboffice will connect to the LibreOffice program at runtime using this variable. It is recommended to define this variable in the `.bashrc` so that it is always present.
    ```
    nano .bashrc
    # Add at the end
    export LIBREOFFICE_HOME=/usr/lib/libreoffice
    # Ctrl+X to save
    source .bashrc
    ```

* Add `$LIBREOFFICE_HOME/program` path to `LD_LIBRARY_PATH` environment variable. In order to run any hboffice-based application, LibreOffice shared libraries `.so` must be accesible and located.
    ```
    # Add at the end of .bashrc
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBREOFFICE_HOME/program
    ```

### Installation in macOS

* Download and install the [LibreOffice.app](https://www.libreoffice.org/download/download-libreoffice/) bundle, typically in the `/Applications` directory.
    ![libreoffice_download_macos](https://github.com/frang75/harbour_nappgui/assets/42999199/1c0bae9a-6751-4ebc-9cc1-463bee184492)
    ![libreoffice_macos_app](https://github.com/frang75/harbour_nappgui/assets/42999199/5b04ca49-d54c-43a9-9db6-b72c9f7dfc1c)

* Set the `LIBREOFFICE_HOME` environment variable with the path to the `LibreOffice.app` bundle (usually `/Applications/LibreOffice.app`). hboffice will connect to the LibreOffice program at runtime using this variable.

* Add `$LIBREOFFICE_HOME$/Contents/Frameworks` path to `DYLD_LIBRARY_PATH` environment variable. In order to run any hboffice-based application, LibreOffice shared libraries `.dylib` must be accesible and located.

    ```
    nano .zshrc
    export LIBREOFFICE_HOME=/Applications/LibreOffice.app
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$LIBREOFFICE_HOME/Contents/Frameworks
    # Ctrl+X to save
    source .zshrc
    ```

* Go to `$LIBREOFFICE_HOME$/Contents/MacOS` and create a symbolic link for `soffice` executable. The bootstrap process looks for `libreoffice` executable.
    ```
    /Applications/LibreOffice.app/Contents/MacOS
    ln -s soffice libreoffice
    ```

> **Important:** The first time a hboffice program uses a LibreOffice function, an instance of the LibreOffice application will be started invisibly (`soffice.bin` process). This first call will have a small delay due to the initialization of the process. It is imperative that LibreOffice is running in order to use the SDK from Harbour.

## Build hboffice

To build the `hboffice` library, just run the `build.bat` or `build.sh` scripts. `officesdk.dll` is already vendored, so **no LibreOffice-SDK or Visual Studio installation is required** for this step.

### Build hboffice in Windows

```
cd contrib\hboffice
build -b [Debug|Release] -comp mingw64
build -b [Debug|Release] -comp clang

:: Set Visual Studio 2026 64bit compiler for hbmk2 (msvc)
"%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
build -b [Debug|Release] -comp msvc64
```

The `libhboffice.a` (mingw64, clang) or `hboffice.lib` (msvc64) will be generated in `hboffice\build\[Debug|Release]\lib`.

### Build hboffice in Linux

```
cd contrib/hboffice
./build.sh -comp gcc -b [Debug|Release]
./build.sh -comp clang -b [Debug|Release]
```

The `libhboffice.a` will be generated in `hboffice/build/[Debug|Release]/lib`.

### Build hboffice in macOS

```
cd contrib/hboffice
export MACOSX_DEPLOYMENT_TARGET=13.0
./build.sh -comp clang -b [Debug|Release]
```

The `libhboffice.a` will be generated in `hboffice/build/[Debug|Release]/lib`.

## hboffice examples

In the `/hboffice/tests` folder there are different examples of use: `sheet1`, `sheet2` (LibreOffice Calc) and `doc1`, `doc2` (LibreOffice Writer).

`hboffice.hbc` already points `hbmk2` at the vendored `officesdk.dll`/`.so`/`.dylib` in `officesdk/bin` at **link** time, so it doesn't need to be copied anywhere to compile the examples. But the resulting executables still need to **find** it at **runtime** (`PATH`/`LD_LIBRARY_PATH`/`DYLD_LIBRARY_PATH`), which is what `test.bat`/`test.sh` set up for you before compiling.

On **Windows**, `tests/test.bat` compiles all four examples and points `PATH` to the vendored officesdk DLL, for any of the three supported compilers.

```
cd contrib\hboffice\tests
test -comp mingw64
test -comp clang

:: Set Visual Studio 2026 64bit compiler for hbmk2 (msvc)
"%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
test -comp msvc64

:: Just run
sheet1.exe
```

On **Linux/macOS**, `tests/test.sh` does the same, setting `LD_LIBRARY_PATH`/`DYLD_LIBRARY_PATH`. Run it with `source` (or `.`) if you want that variable to stay set in your shell afterwards, to run the resulting executables right away:

```
cd contrib/hboffice/tests
source test.sh -comp gcc
source test.sh -comp clang

# Just run
./sheet1
```

The results documents will be saved in `/tests/result`.

> **Important:** the vendored `officesdk.dll`/`.so`/`.dylib` used by `test.bat`/`test.sh` must still be distributed alongside your own final executables (copy it next to the binary, or make sure it's on `PATH`/`LD_LIBRARY_PATH`/`DYLD_LIBRARY_PATH`).

## For hboffice developers

If you need to modify `officesdk` itself (the LibreOffice-SDK C++ wrapper) or refresh the vendored `officesdk.dll`/`.so`/`.dylib`, see [officesdk/Readme.md](./officesdk/Readme.md). This requires Visual Studio (Windows) and the LibreOffice-SDK, and is **not** needed just to consume `hboffice`.
