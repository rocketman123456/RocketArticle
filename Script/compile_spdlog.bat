mkdir Thirdparty\build\spdlog
pushd Thirdparty\build\spdlog
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\spdlog
cmake --build . --config Release --target install
popd