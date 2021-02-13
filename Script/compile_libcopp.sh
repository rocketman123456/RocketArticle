mkdir -p Thirdparty/build/libcopp
pushd Thirdparty/build/libcopp
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../libcopp
cmake --build . --config Debug --target install
popd