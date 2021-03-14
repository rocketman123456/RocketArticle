mkdir -p Thirdparty/build/CSerialPort
pushd Thirdparty/build/CSerialPort
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../CSerialPort
cmake --build . --config Debug --target install
popd