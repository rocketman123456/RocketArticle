mkdir -p Thirdparty/build/glm
pushd Thirdparty/build/glm
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DGLM_TEST_ENABLE=ON ../../glm
cmake --build . --config Debug --target install
popd