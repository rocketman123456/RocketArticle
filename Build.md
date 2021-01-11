macos<br>
```
pushd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/Users/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
popd
```
linux<br>
```
pushd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
popd
```
windows<br>
```
pushd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
popd
```