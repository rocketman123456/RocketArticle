#include "Vulkan/Render/VulkanShader.h"
#include "Vulkan/Render/VulkanFunction.h"
#include "Module/Application.h"
#include "Module/AssetLoader.h"

using namespace Rocket;

// Returns GLSL shader source text after preprocessing.
static String PreprocessShader(
    const String& source_name,
    shaderc_shader_kind kind,
    const String& source)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    //options.AddMacroDefinition("MY_DEFINE", "1");

    shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(source, kind, source_name.c_str(), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        RK_GRAPHICS_ERROR(result.GetErrorMessage());
        return "";
    }

    return {result.cbegin(), result.cend()};
}

// Compiles a shader to SPIR-V assembly.
static String CompileFileToAssembly(
    const String& source_name,
    shaderc_shader_kind kind,
    const String& source,
    bool optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    //options.AddMacroDefinition("MY_DEFINE", "1");
    if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(source, kind, source_name.c_str(), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        RK_GRAPHICS_ERROR(result.GetErrorMessage());
        return "";
    }

    return {result.cbegin(), result.cend()};
}

// Compiles SPIR-V assembly to binary.
static Vec<uint32_t> CompileAssemblyToBinary(
    const String& source, 
    bool optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    //options.AddMacroDefinition("MY_DEFINE", "1");
    if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::SpvCompilationResult result = compiler.AssembleToSpv(source, options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        RK_GRAPHICS_ERROR(result.GetErrorMessage());
        return Vec<uint32_t>();
    } 

    return {result.cbegin(), result.cend()};
}

// Compiles a shader to a SPIR-V binary.
static Vec<uint32_t> CompileFile(
    const String& source_name,
    shaderc_shader_kind kind,
    const String& source,
    bool optimize = false)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    //options.AddMacroDefinition("MY_DEFINE", "1");
    if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        RK_GRAPHICS_ERROR(module.GetErrorMessage());
        return Vec<uint32_t>();
    }

    return {module.cbegin(), module.cend()};
}

bool VulkanShader::Initialize(const ShaderSourceList& list)
{
    for (auto it = list.cbegin(); it != list.cend(); it++)
    {
        if (!it->second.empty())
        {
            VkShaderModule shader = VK_NULL_HANDLE;
            if (!is_binary_)
            {
                auto config = g_Application->GetConfig();
                String full_path = String("Shaders/" + RenderAPI + "/" + it->second);

                String shaderBuffer = g_AssetLoader->SyncOpenAndReadTextFileToString(full_path);
                // Preprocessing
                String preprocessed = PreprocessShader("shader_src", (shaderc_shader_kind)it->first, shaderBuffer);
                //RK_GRAPHICS_TRACE("Compiled a vertex shader resulting in preprocessed text:\n {}", preprocessed);
                // Optimizing
                String assembly = CompileFileToAssembly("shader_preprocessed", (shaderc_shader_kind)it->first, preprocessed, true);
                //RK_GRAPHICS_TRACE("Optimized SPIR-V assembly:\n {}", assembly);
                // Compile
                Vec<uint32_t> spirv = CompileAssemblyToBinary(assembly, true);
                RK_GRAPHICS_INFO("Compiled to a binary module with {} words.", spirv.size());

                shader = CreateShaderModule(spirv, device_handle_);
            }
            else
            {
                // Load Precompiled shader
                auto config = g_Application->GetConfig();
                String full_path = String("Shaders/" + RenderAPI + "/" + it->second + ".spv");

                Buffer shaderBuffer = g_AssetLoader->SyncOpenAndReadBinary(full_path);
                
                shader = CreateShaderModule(shaderBuffer, device_handle_);
            }

            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.module = shader;
            shaderStageInfo.pName = "main";

            switch (it->first)
            {
            case shaderc_glsl_vertex_shader:
                shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case shaderc_glsl_fragment_shader:
                shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case shaderc_glsl_compute_shader:
                shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            case shaderc_glsl_geometry_shader:
                shaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                break;
            case shaderc_glsl_tess_control_shader:
                shaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                break;
            case shaderc_glsl_tess_evaluation_shader:
                shaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                break;
            }

            shaders_.emplace_back(shader);
            shader_infos_.emplace_back(shaderStageInfo);
        }
    }
    return true;
}

void VulkanShader::Finalize()
{
    for (auto shader : shaders_)
    {
        vkDestroyShaderModule(device_handle_, shader, nullptr);
    }
}

void VulkanShader::Bind() const
{
}

void VulkanShader::Unbind() const
{
}

bool VulkanShader::SetInt32(const String& name, int32_t value)
{
    return true;
}

bool VulkanShader::SetUInt32(const String& name, uint32_t value)
{
    return true;
}

bool VulkanShader::SetInt32Array(const String& name, int32_t* values, uint32_t count)
{
    return true;
}

bool VulkanShader::SetUInt32Array(const String& name, uint32_t* values, uint32_t count)
{
    return true;
}

bool VulkanShader::SetFloat(const String& name, float value)
{
    return true;
}

bool VulkanShader::SetDouble(const String& name, double value)
{
    return true;
}

bool VulkanShader::SetVector2f(const String& name, const Vector2f& value)
{
    return true;
}

bool VulkanShader::SetVector3f(const String& name, const Vector3f& value)
{
    return true;
}

bool VulkanShader::SetVector4f(const String& name, const Vector4f& value)
{
    return true;
}

bool VulkanShader::SetMatrix2f(const String& name, const Matrix2f& value)
{
    return true;
}

bool VulkanShader::SetMatrix3f(const String& name, const Matrix3f& value)
{
    return true;
}

bool VulkanShader::SetMatrix4f(const String& name, const Matrix4f& value)
{
    return true;
}