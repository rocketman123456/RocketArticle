mkdir -p Thirdparty/build/marl
pushd Thirdparty/build/marl
rm -rf *
cmake -G "Ninja" -DMARL_INSTALL=ON -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../marl
cmake --build . --config Debug --target install
popd