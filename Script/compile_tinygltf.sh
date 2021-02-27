mkdir -p Thirdparty/build/tinygltf
pushd Thirdparty/build/tinygltf
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DTINYGLTF_BUILD_LOADER_EXAMPLE=OFF ../../tinygltf
cmake --build . --config Debug --target install
popd