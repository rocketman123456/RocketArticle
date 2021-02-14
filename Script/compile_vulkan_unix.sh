./External/`uname -s`/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/base.vert -o Asset/Shaders/Vulkan/base_vert.spv
./External/`uname -s`/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/base.frag -o Asset/Shaders/Vulkan/base_frag.spv
./External/`uname -s`/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/draw2d.vert -o Asset/Shaders/Vulkan/draw2d_vert.spv
./External/`uname -s`/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/draw2d.frag -o Asset/Shaders/Vulkan/draw2d_frag.spv
echo "Compile Shader Complete"