#include "Vulkan/VulkanShader.h"
#include "Module/Application.h"
#include "Module/AssetLoader.h"

using namespace Rocket;

// Returns GLSL shader source text after preprocessing.
static String PreprocessShader(const String& source_name,
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
static String CompileFileToAssembly(const String& source_name,
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
static Vec<uint32_t> CompileAssemblyToBinary(const String& source, bool optimize = false)
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
static Vec<uint32_t> CompileFile(const String& source_name,
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
            auto config = g_Application->GetConfig();
            auto full_path = ("Shaders/" + RenderAPI + "/" + it->second);
            RK_GRAPHICS_INFO("Load Shader File {}", full_path);
            String shaderBuffer = g_AssetLoader->SyncOpenAndReadTextFileToString(full_path);
            // Preprocessing
            auto preprocessed = PreprocessShader("shader_src", (shaderc_shader_kind)it->first, shaderBuffer);
            //RK_GRAPHICS_INFO("Compiled a vertex shader resulting in preprocessed text:\n {}", preprocessed);
            // Optimizing
            auto assembly = CompileFileToAssembly("shader_preprocessed", (shaderc_shader_kind)it->first, preprocessed, true);
            //RK_GRAPHICS_INFO("Optimized SPIR-V assembly:\n {}", assembly);
            // Compile
            auto spirv = CompileAssemblyToBinary(assembly, true);
            RK_GRAPHICS_INFO("Compiled to a binary module with {} words.", spirv.size());
        }
    }
    return true;
}

void VulkanShader::Finalize()
{
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