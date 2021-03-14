mkdir Thirdparty\build\marl
pushd Thirdparty\build\marl
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DMARL_INSTALL=ON -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\marl
cmake --build . --config Debug --target install
popd