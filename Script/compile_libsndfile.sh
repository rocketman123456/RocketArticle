mkdir -p Thirdparty/build/libsndfile
pushd Thirdparty/build/libsndfile
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DENABLE_MPEG=OFF -DBUILD_EXAMPLES=OFF -DENABLE_EXTERNAL_LIBS=OFF -DBUILD_REGTEST=OFF ../../libsndfile
cmake --build . --config Debug --target install
popd