@ECHO OFF

set VC_EDITION=Community
set VC_VERSION=2019
set VC_ARCH=x86
set BM_BUILD_DIR=build-release-x86

where /Q cl.exe &^
if ERRORLEVEL 1 (
    ECHO Initializing MSVC '%VC_ARCH%' environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\%VC_VERSION%\%VC_EDITION%\VC\Auxiliary\Build\vcvarsall.bat" %VC_ARCH%
) else (
    ECHO MSVC '%VC_ARCH%' environment already initialized.
)

@REM Copy our cimgui support files into thirdparty/cimgui directory.
@REM This customizes cimgui to use SDL2, DX11 and WIN32 backends.
xcopy /Y thirdparty\cimgui-support\CMakeLists.txt thirdparty\cimgui\CMakeLists.txt
xcopy /Y thirdparty\cimgui-support\generator.bat thirdparty\cimgui\generator\generator.bat
cd thirdparty\cimgui
rmdir /s /q build
cd ..\..\

@REM Build everything.
rmdir /s /q %BM_BUILD_DIR%
cmake -S. -B%BM_BUILD_DIR% -GNinja -DBM_BUILD_32BIT=ON -DCMAKE_BUILD_TYPE=Release -DIMGUI_STATIC=TRUE -DBUILD_SHARED_LIBS=OFF
cmake --build %BM_BUILD_DIR%
