mkdir Thirdparty\build\tinygltf
pushd Thirdparty\build\tinygltf
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DTINYGLTF_BUILD_LOADER_EXAMPLE=OFF ..\..\tinygltf
cmake --build . --config Release --target install
popd