mkdir Thirdparty/build/entt
pushd Thirdparty/build/entt
rm -rf *
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=../../../External/Windows/ -DCMAKE_INSTALL_RPATH=../../../External/Windows/ ../../entt
cmake --build . --config Release --target install
popd