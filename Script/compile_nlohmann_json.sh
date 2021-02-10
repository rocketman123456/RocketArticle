mkdir -p Thirdparty/build/nlohmann-json
pushd Thirdparty/build/nlohmann-json
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../nlohmann-json
cmake --build . --config Debug --target install
popd