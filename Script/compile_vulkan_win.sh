./External/Windows/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/shader_base.vert -o Asset/Shaders/Vulkan/base_vert.spv
./External/Windows/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/shader_base.frag -o Asset/Shaders/Vulkan/base_frag.spv
./External/Windows/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/draw2d.vert -o Asset/Shaders/Vulkan/draw2d_vert.spv
./External/Windows/bin/glslangValidator -e main -V Asset/Shaders/Vulkan/draw2d.frag -o Asset/Shaders/Vulkan/draw2d_frag.spv
echo "Compile Shader Complete"