## Prebuild binaries of AWSSDK and OFFICESDK

Starting 05/31/2026 (Sprint 106) I will be providing prebuilt versions of the AWSSDK and OFFICESDK. This will greatly simplify the compilation and deployment of Harbour projects based on HBAWS and HBOFFICE. What should you take into account? Let's go by steps:

### AWSSDK

**From now on it is not necessary**:

- No need to run `awssdk -b Release -comp mingw64`. As you know, this is a very heavy script that requires you to download AWS-SDK from GitHub, patch it and compile it for VS, MingGW, GCC, Clang.

- How to work now:

    * Download the precompiled version of `awssdk`.
    * Copy the binaries to `AWS_SDK_ROOT`.
    * **NEVER run the script** `awssdk -b Release -comp mingw64`. This will delete the prebuilt binaries and download the AWS-SDK to recompile.
    * Continue as before, compiling HBAWS and the final applications as before.

- Future improvements.

    * Create a dll awssdk.dll that encapsulates calls to the aws-sdk. With this we will avoid having to precompile aws-sdk for each compiler. It can be compiled for VS2026 and reuse the dll in HBAWS, abstracting the compiler, in the same way we do in HBOFFICE.

### OFFICE SDK

**From now on it is not necessary**:

- Have the LibreOffice-SDK installed. This is already embedded in `officesdk.dll`. It is only necessary to have LibreOffice, both on the development machines and on the end user machines.

- No need to run anymore `build -dll -b Release`.

- How to work now:

    * Download the precompiled version of `officesdk.dll`.
    * Copy `officesdk.dll/officesdk.lib` to `/hboffice/build/Release/lib`. This is necessary for HBOFFICE/MingGW applications to link with LibreOffice.
    * Compile `hboffice` as before:
        `build -lib -comp mingw64 -b Release`
    * Compile the application based on HBOFFICE, as before.
    * Distribute `officesdk.dll` with the final application, as before.
