mkdir Thirdparty/build/nlohmann-json
pushd Thirdparty/build/nlohmann-json
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../nlohmann-json
cmake --build . --config Release --target install
popd