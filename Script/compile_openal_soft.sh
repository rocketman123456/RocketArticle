mkdir -p Thirdparty/build/openal-soft
pushd Thirdparty/build/openal-soft
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/  -DALSOFT_EXAMPLES=OFF -DALSOFT_INSTALL_EXAMPLES=OFF ../../openal-soft
cmake --build . --config Debug --target install
popd