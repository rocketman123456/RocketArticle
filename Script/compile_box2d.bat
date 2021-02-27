mkdir Thirdparty\build\box2d
pushd Thirdparty\build\box2d
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DBOX2D_BUILD_UNIT_TESTS=OFF -DBOX2D_BUILD_TESTBED=OFF ..\..\box2d
cmake --build . --config Debug --target install
popd