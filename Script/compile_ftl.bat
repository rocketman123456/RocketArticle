mkdir Thirdparty\build\FiberTaskingLib
pushd Thirdparty\build\FiberTaskingLib
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DFTL_BUILD_TESTS=OFF -DFTL_BUILD_BENCHMARKS=OFF -DFTL_BUILD_EXAMPLES=OFF ..\..\FiberTaskingLib
cmake --build . --config Release --target install
popd