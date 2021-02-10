mkdir -p Thirdparty/build/yaml-cpp
pushd Thirdparty/build/yaml-cpp
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DYAML_CPP_BUILD_TESTS=OFF  ../../yaml-cpp
cmake --build . --config Release --target install
popd