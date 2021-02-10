mkdir -p Thirdparty/build/eigen
pushd Thirdparty/build/eigen
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../eigen
cmake --build . --config Release --target install
popd