mkdir ninja-release-x64
cd ninja-release-x64
cmake -DCMAKE_BUILD_TYPE=Release -GNinja ..
ninja -f build.ninja
