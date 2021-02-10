mkdir -p Thirdparty/build/bullet
pushd Thirdparty/build/bullet
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ -DBUILD_SHARED_LIBS=OFF -DBUILD_BULLET2_DEMOS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_CPU_DEMOS=OFF -DBUILD_PYBULLET=OFF -DUSE_DOUBLE_PRECISION=ON -DBUILD_EXTRAS=OFF -DBUILD_UNIT_TESTS=OFF -DINSTALL_LIBS=ON ../../bullet3
cmake --build . --config Release --target install
popd