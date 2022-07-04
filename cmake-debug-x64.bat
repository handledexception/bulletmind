@ECHO OFF

set VC_EDITION=Community
set VC_VERSION=2019
set VC_ARCH=amd64
set BM_BUILD_DIR=ninja-debug-x64

where /Q cl.exe &^
if ERRORLEVEL 1 (
    ECHO Initializing MSVC '%VC_ARCH%' environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\%VC_VERSION%\%VC_EDITION%\VC\Auxiliary\Build\vcvarsall.bat" %VC_ARCH%
) else (
    ECHO MSVC '%VC_ARCH%' environment already initialized.
)

rmdir /s /q %BM_BUILD_DIR%
mkdir %BM_BUILD_DIR%
cd %BM_BUILD_DIR%
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
ninja -f build.ninja
