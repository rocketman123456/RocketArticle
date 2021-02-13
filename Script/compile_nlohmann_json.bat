mkdir Thirdparty\build\nlohmann-json
pushd Thirdparty\build\nlohmann-json
del /S /Q *
cmake -G "Visual Studio 16 2019" -A "x64" -Thost=x64 -DCMAKE_INSTALL_PREFIX=..\..\..\External\Windows\ -DCMAKE_INSTALL_RPATH=..\..\..\External\Windows\ -DJSON_BuildTests=OFF ..\..\nlohmann-json
cmake --build . --config Debug --target install
popd