mkdir -p Thirdparty/build/shaderc
pushd Thirdparty/shaderc
./utils/git-sync-deps
popd
pushd Thirdparty/build/shaderc
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DSHADERC_SKIP_TESTS=ON -DSHADERC_SKIP_EXAMPLES=ON ../../shaderc
cmake --build . --config Debug --target install
popd