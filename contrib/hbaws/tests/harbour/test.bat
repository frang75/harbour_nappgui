::
:: HBAWS Harbour examples build script
::
:: Compiles all the example .prg tests and sets PATH to the AWS-SDK DLLs
:: required by the resulting executables.
::
:: test -comp [mingw64|clang|msvc64]
::

@echo off

::
:: Input parameters
::
set COMPILER=mingw64
set "CWD=%cd%"

:parse
IF "%~1"=="" GOTO endparse
IF "%~1"=="-comp" GOTO compiler
SHIFT
GOTO parse

:compiler
SHIFT
set COMPILER=%~1
SHIFT
GOTO parse

:endparse

::
:: Beginning
::
echo ---------------------------
echo Compiling HBAWS Harbour examples
echo COMPILER: %COMPILER%
echo ---------------------------

set HBMK_PATH=..\..\..\..\bin\win\%COMPILER%

:: AWS-SDK vendored DLLs. MSVC and mingw64/clang binaries are not
:: ABI-compatible, so each compiler needs its own folder in PATH.
IF "%COMPILER%"=="msvc64" (
    set "PATH=%CWD%\..\..\awssdk\bin\win\msvc;%PATH%"
) ELSE (
    set "PATH=%CWD%\..\..\awssdk\bin\win\gcc;%PATH%"
)

for %%P in (listall listpage upload uploadm copy copym download delete restore) do (
    echo Compiling %%P.prg...
    call %HBMK_PATH%\hbmk2.exe %%P.prg credentials.prg hbaws.hbc -comp=%COMPILER% || goto error_hbaws
)

echo ------------------------
echo HBAWS examples build succeed
echo ------------------------
goto end

::
:: Errors
::

:error_hbaws
echo Error building HBAWS examples
goto end

:end
