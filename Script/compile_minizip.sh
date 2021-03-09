mkdir -p Thirdparty/build/minizip
pushd Thirdparty/build/minizip
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../minizip-ng
cmake --build . --config Debug --target install
popd
