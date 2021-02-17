#include "Render/DrawBasic/Shader.h"
#include "Utils/Hashing.h"

#include <vulkan/vulkan.h>

namespace Rocket
{
    ENUM(SHADER_TYPE)
    {
        // Forced shader kinds. These shader kinds force the compiler to compile the
        // source code as the specified kind of shader.
        shaderc_vertex_shader = 0,
        shaderc_fragment_shader,
        shaderc_compute_shader,
        shaderc_geometry_shader,
        shaderc_tess_control_shader,
        shaderc_tess_evaluation_shader,

        shaderc_glsl_vertex_shader = shaderc_vertex_shader,
        shaderc_glsl_fragment_shader = shaderc_fragment_shader,
        shaderc_glsl_compute_shader = shaderc_compute_shader,
        shaderc_glsl_geometry_shader = shaderc_geometry_shader,
        shaderc_glsl_tess_control_shader = shaderc_tess_control_shader,
        shaderc_glsl_tess_evaluation_shader = shaderc_tess_evaluation_shader,

        // Deduce the shader kind from #pragma annotation in the source code. Compiler
        // will emit error if #pragma annotation is not found.
        shaderc_glsl_infer_from_source,
        // Default shader kinds. Compiler will fall back to compile the source code as
        // the specified kind of shader when #pragma annotation is not found in the
        // source code.
        shaderc_glsl_default_vertex_shader,
        shaderc_glsl_default_fragment_shader,
        shaderc_glsl_default_compute_shader,
        shaderc_glsl_default_geometry_shader,
        shaderc_glsl_default_tess_control_shader,
        shaderc_glsl_default_tess_evaluation_shader,
        shaderc_spirv_assembly,
        shaderc_raygen_shader,
        shaderc_anyhit_shader,
        shaderc_closesthit_shader,
        shaderc_miss_shader,
        shaderc_intersection_shader,
        shaderc_callable_shader,
        shaderc_glsl_raygen_shader = shaderc_raygen_shader,
        shaderc_glsl_anyhit_shader = shaderc_anyhit_shader,
        shaderc_glsl_closesthit_shader = shaderc_closesthit_shader,
        shaderc_glsl_miss_shader = shaderc_miss_shader,
        shaderc_glsl_intersection_shader = shaderc_intersection_shader,
        shaderc_glsl_callable_shader = shaderc_callable_shader,
        shaderc_glsl_default_raygen_shader,
        shaderc_glsl_default_anyhit_shader,
        shaderc_glsl_default_closesthit_shader,
        shaderc_glsl_default_miss_shader,
        shaderc_glsl_default_intersection_shader,
        shaderc_glsl_default_callable_shader,
        shaderc_task_shader,
        shaderc_mesh_shader,
        shaderc_glsl_task_shader = shaderc_task_shader,
        shaderc_glsl_mesh_shader = shaderc_mesh_shader,
        shaderc_glsl_default_task_shader,
        shaderc_glsl_default_mesh_shader,
    };

    class VulkanShader : implements Shader
    {
    public:
        VulkanShader(const String& name) : m_NameId(AssetHashTable::HashString(name)) {}
        VulkanShader(const VulkanShader& rhs) = default;
        VulkanShader(VulkanShader&& rhs) = default;
        virtual ~VulkanShader() = default;

        void SetDevice(VkDevice& device) { m_DeviceHandle = device; }

        bool Initialize(const ShaderSourceList& list) final;
		void Finalize() final;

        void Bind() const final;
		void Unbind() const final;

		bool SetInt32(const String& name, int32_t value) final;
        bool SetUInt32(const String& name, uint32_t value) final;
		bool SetInt32Array(const String& name, int32_t* values, uint32_t count) final;
        bool SetUInt32Array(const String& name, uint32_t* values, uint32_t count) final;

		bool SetFloat(const String& name, float value) final;
        bool SetDouble(const String& name, double value) final;

		bool SetVector2f(const String& name, const Vector2f& value) final;
		bool SetVector3f(const String& name, const Vector3f& value) final;
		bool SetVector4f(const String& name, const Vector4f& value) final;

        bool SetMatrix2f(const String& name, const Matrix2f& value) final;
		bool SetMatrix3f(const String& name, const Matrix3f& value) final;
		bool SetMatrix4f(const String& name, const Matrix4f& value) final;

        const String& GetName() const final { return AssetHashTable::GetStringFromId(m_NameId); }
		uint32_t GetRenderId() const final { return m_RendererId; }

        Vec<VkShaderModule>& GetShader() { return m_Shader; }
        Vec<VkPipelineShaderStageCreateInfo>& GetShaderInfo() { return m_ShaderInfo; }
    private:
        uint64_t m_NameId;
        uint32_t m_RendererId;
        Vec<VkShaderModule> m_Shader;
        Vec<VkPipelineShaderStageCreateInfo> m_ShaderInfo;
        VkDevice m_DeviceHandle;
    };
}