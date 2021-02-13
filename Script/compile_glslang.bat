mkdir Thirdparty\build\glslang
pushd Thirdparty\build\glslang
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\glslang
cmake --build . --config Debug --target install
popd