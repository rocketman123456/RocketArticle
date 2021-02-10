mkdir Thirdparty\build\taskflow
pushd Thirdparty\build\taskflow
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DTF_BUILD_EXAMPLES=OFF -DTF_BUILD_TESTS=OFF ..\..\taskflow
cmake --build . --config Debug --target install
popd