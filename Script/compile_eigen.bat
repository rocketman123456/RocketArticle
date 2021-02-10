mkdir Thirdparty/build/eigen
pushd Thirdparty/build/eigen
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../eigen
cmake --build . --config Release --target install
popd