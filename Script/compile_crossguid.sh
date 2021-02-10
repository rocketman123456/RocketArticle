mkdir -p Thirdparty/build/crossguid
pushd Thirdparty/build/crossguid
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../crossguid
cmake --build . --config Debug --target install
popd