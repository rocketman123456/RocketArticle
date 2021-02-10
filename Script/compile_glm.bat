mkdir Thirdparty/build/glm
pushd Thirdparty/build/glm
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ -DGLM_TEST_ENABLE=ON ../../glm
cmake --build . --config Release --target install
popd