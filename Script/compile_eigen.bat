mkdir Thirdparty\build\eigen
pushd Thirdparty\build\eigen
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\eigen
cmake --build . --config Debug --target install
popd