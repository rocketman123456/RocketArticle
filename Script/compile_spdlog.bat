mkdir Thirdparty/build/spdlog
pushd Thirdparty/build/spdlog
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../spdlog
cmake --build . --config Release --target install
popd