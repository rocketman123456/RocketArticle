mkdir Thirdparty\build\openal-soft
pushd Thirdparty\build\openal-soft
rm -rf *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\  -DALSOFT_EXAMPLES=OFF -DALSOFT_INSTALL_EXAMPLES=OFF ..\..\openal-soft
cmake --build . --config Debug --target install
popd