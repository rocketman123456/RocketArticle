mkdir Thirdparty\build\shaderc
pushd Thirdparty\build\shaderc
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DSHADERC_SKIP_TESTS=ON -DSHADERC_SKIP_EXAMPLES=ON -DSHADERC_ENABLE_SHARED_CRT=ON ..\..\shaderc
cmake --build . --config Debug --target install
popd