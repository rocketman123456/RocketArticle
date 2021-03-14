mkdir Thirdparty\build\gli
pushd Thirdparty\build\gli
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DGLI_TEST_ENABLE=OFF -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\gli
cmake --build . --config Debug --target install
popd