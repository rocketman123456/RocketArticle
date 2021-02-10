mkdir Thirdparty/build/glslang
pushd Thirdparty/build/glslang
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../glslang
cmake --build . --config Release --target install
popd