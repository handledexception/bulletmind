mkdir ninja-debug-x86
cd ninja-debug-x86
cmake -DCMAKE_BUILD_TYPE=Debug -DBM_BUILD_32BIT=ON -DBM_BUILD_64BIT=OFF -GNinja ..
