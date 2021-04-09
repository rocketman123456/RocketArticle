echo "./Script/compile_box2d.bat"
echo "./Script/compile_bullet.bat"
echo "./Script/compile_crossguid.bat"
echo "./Script/compile_eigen.bat"
echo "./Script/compile_entt.bat"
echo "./Script/compile_glfw.bat"
echo "./Script/compile_gli.bat"
echo "./Script/compile_glslang.bat"
echo "./Script/compile_libcopp.bat"
echo "./Script/compile_libsndfile.bat"
echo "./Script/compile_nlohmann_json.bat"
echo "./Script/compile_openal_soft.bat"
echo "./Script/compile_shaderc.bat"
echo "./Script/compile_spdlog.bat"
echo "./Script/compile_taskflow.bat"
echo "./Script/compile_tinygltf.bat"
echo "./Script/compile_tinyobjloader.bat"
echo "./Script/compile_yaml_cpp.bat"
mkdir build
pushd build
del /S /Q *
cmake ..
#cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug
popd