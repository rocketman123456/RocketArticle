mkdir Thirdparty\build\crossguid
pushd Thirdparty\build\crossguid
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\crossguid
cmake --build . --config Debug --target install
popd