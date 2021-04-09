mkdir build
pushd build
rm -rf *
cmake ..
#cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
popd