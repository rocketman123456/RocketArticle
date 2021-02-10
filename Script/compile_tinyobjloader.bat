mkdir Thirdparty\build\tinyobjloader
pushd Thirdparty\build\tinyobjloader
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\tinyobjloader
cmake --build . --config Release --target install
popd