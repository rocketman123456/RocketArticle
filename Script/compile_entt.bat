mkdir Thirdparty\build\entt
pushd Thirdparty\build\entt
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\entt
cmake --build . --config Debug --target install
popd