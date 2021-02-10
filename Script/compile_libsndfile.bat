mkdir Thirdparty/build/libsndfile
pushd Thirdparty/build/libsndfile
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ -DBUILD_EXAMPLES=OFF ../../libsndfile
cmake --build . --config Release --target install
popd