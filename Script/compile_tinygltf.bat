mkdir Thirdparty/build/tinygltf
pushd Thirdparty/build/tinygltf
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ -DTINYGLTF_BUILD_LOADER_EXAMPLE=OFF ../../tinygltf
cmake --build . --config Release --target install
popd