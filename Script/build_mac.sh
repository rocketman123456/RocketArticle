pushd build
rm -rf *
cmake .. -DCMAKE_TOOLCHAIN_FILE=/Users/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
popd