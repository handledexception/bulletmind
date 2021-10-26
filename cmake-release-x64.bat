mkdir ninja-release-x64
cd ninja-release-x64
cmake -DCMAKE_BUILD_TYPE=Release -DBM_BUILD_32BIT=OFF -GNinja ..
ninja -f build.ninja
