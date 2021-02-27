mkdir -p Thirdparty/build/spdlog
pushd Thirdparty/build/spdlog
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../spdlog
cmake --build . --config Debug --target install
popd