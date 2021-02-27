mkdir Thirdparty\build\tinygltf
pushd Thirdparty\build\tinygltf
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DTINYGLTF_BUILD_LOADER_EXAMPLE=OFF ..\..\tinygltf
cmake --build . --config Debug --target install
popd