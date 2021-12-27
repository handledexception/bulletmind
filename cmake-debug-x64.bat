REM call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
mkdir ninja-debug-x64
cd ninja-debug-x64
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
ninja -f build.ninja
