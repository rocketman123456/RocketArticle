mkdir -p Thirdparty/build/libsndfile
pushd Thirdparty/build/libsndfile
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DBUILD_EXAMPLES=OFF ../../libsndfile
cmake --build . --config Release --target install
popd