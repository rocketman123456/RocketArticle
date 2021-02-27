mkdir -p Thirdparty/build/tinyobjloader
pushd Thirdparty/build/tinyobjloader
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../tinyobjloader
cmake --build . --config Debug --target install
popd