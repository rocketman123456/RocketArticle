mkdir -p Thirdparty/build/entt
pushd Thirdparty/build/entt
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/`uname -s`/ -DCMAKE_INSTALL_RPATH=../../../External/`uname -s`/ ../../entt
cmake --build . --config Debug --target install
popd