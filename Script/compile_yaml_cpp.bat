mkdir Thirdparty/build/yaml-cpp
pushd Thirdparty/build/yaml-cpp
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ -DYAML_CPP_BUILD_TESTS=OFF  ../../yaml-cpp
cmake --build . --config Release --target install
popd