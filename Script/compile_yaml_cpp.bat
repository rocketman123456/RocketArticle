mkdir Thirdparty\build\yaml-cpp
pushd Thirdparty\build\yaml-cpp
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DYAML_CPP_BUILD_TESTS=OFF -DYAML_MSVC_SHARED_RT=ON ..\..\yaml-cpp
cmake --build . --config Debug --target install
popd