mkdir -p Thirdparty/build/gli
pushd Thirdparty/build/gli
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../gli
cmake --build . --config Debug --target install
popd