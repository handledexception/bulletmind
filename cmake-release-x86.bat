mkdir ninja-release-x86
cd ninja-release-x86
cmake -DCMAKE_BUILD_TYPE=Release -DBM_BUILD_32BIT=ON -DBM_BUILD_64BIT=OFF -GNinja ..