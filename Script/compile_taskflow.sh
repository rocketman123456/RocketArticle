mkdir -p Thirdparty/build/taskflow
pushd Thirdparty/build/taskflow
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DTF_BUILD_EXAMPLES=OFF -DTF_BUILD_TESTS=OFF ../../taskflow
cmake --build . --config Debug --target install
popd