mkdir ninja-debug-x64
cd ninja-debug-x64
cmake -DCMAKE_BUILD_TYPE=Debug -DBM_BUILD_32BIT=OFF -GNinja ..
ninja -f build.ninja
