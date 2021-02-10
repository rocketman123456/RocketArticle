mkdir Thirdparty/build/tinyobjloader
pushd Thirdparty/build/tinyobjloader
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../tinyobjloader
cmake --build . --config Release --target install
popd