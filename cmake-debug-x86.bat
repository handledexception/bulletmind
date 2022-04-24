call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
mkdir ninja-debug-x86
cd ninja-debug-x86
cmake -DCMAKE_BUILD_TYPE=Debug -DBM_BUILD_32BIT=ON -GNinja ..
ninja -f build.ninja
