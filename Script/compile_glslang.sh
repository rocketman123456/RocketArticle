mkdir -p Thirdparty/build/glslang
pushd Thirdparty/build/glslang
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../glslang
cmake --build . --config Debug --target install
popd