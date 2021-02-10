mkdir Thirdparty\build\glm
pushd Thirdparty\build\glm
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DGLM_TEST_ENABLE=ON ..\..\glm
cmake --build . --config Debug --target install
popd