mkdir Thirdparty\build\CSerialPort
pushd Thirdparty\build\CSerialPort
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ ..\..\CSerialPort
cmake --build . --config Debug --target install
popd