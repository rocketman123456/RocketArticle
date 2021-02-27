mkdir -p Thirdparty/build/box2d
pushd Thirdparty/build/box2d
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DBOX2D_BUILD_UNIT_TESTS=OFF -DBOX2D_BUILD_TESTBED=OFF ../../box2d
cmake --build . --config Debug --target install
popd