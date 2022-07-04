@ECHO OFF

set VC_EDITION=Community
set VC_VERSION=2019
set VC_ARCH=x86
set BM_BUILD_DIR=ninja-debug-x86

where /Q cl.exe &^
if ERRORLEVEL 1 (
    ECHO Initializing MSVC '%VC_ARCH%' environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\%VC_VERSION%\%VC_EDITION%\VC\Auxiliary\Build\vcvarsall.bat" %VC_ARCH%
) else (
    ECHO MSVC '%VC_ARCH%' environment already initialized.
)

rmdir /s /q %BM_BUILD_DIR%
cmake -DCMAKE_BUILD_TYPE=Debug -DBM_BUILD_32BIT=ON -S. -B%BM_BUILD_DIR% -GNinja
ninja -C %BM_BUILD_DIR%
