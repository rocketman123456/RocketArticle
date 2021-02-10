mkdir -p Thirdparty/build/FiberTaskingLib
pushd Thirdparty/build/FiberTaskingLib
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DFTL_BUILD_TESTS=OFF -DFTL_BUILD_BENCHMARKS=OFF -DFTL_BUILD_EXAMPLES=OFF ../../FiberTaskingLib
cmake --build . --config Release --target install
popd