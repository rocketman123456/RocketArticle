mkdir Thirdparty/build/crossguid
pushd Thirdparty/build/crossguid
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../crossguid
cmake --build . --config Release --target install
popd