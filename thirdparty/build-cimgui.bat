@ECHO ON

REM This batch file must be run from the thirdparty dir!
REM LuaJit must be present in thirdparty\luapower-all-master
REM Download LuaJit from: https://github.com/luapower/all/archive/master.zip

SET VC_EDITION=Community
SET VC_VERSION=2019
SET VC_ARCH=amd64
SET BUILD_TYPE=Debug

where /Q cl.exe &^
if ERRORLEVEL 1 (
    ECHO Initializing MSVC '%VC_ARCH%' environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\%VC_VERSION%\%VC_EDITION%\VC\Auxiliary\Build\vcvarsall.bat" %VC_ARCH%
) else (
    ECHO MSVC '%VC_ARCH%' environment already initialized.
)

COPY /Y .\cimgui\generator\generator.bat .\cimgui\generator\generator.bak
COPY /Y .\cimgui-support\generator.bat .\cimgui\generator\generator.bat
COPY /Y .\cimgui-support\CMakeLists.txt .\cimgui\CMakeLists.txt

CD cimgui\generator
call generator.bat
CD ..

rmdir /s /q build
cmake -S. -Bbuild -GNinja -DIMGUI_STATIC=ON -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
cmake --build build
CD ..
