#pragma once
#include "Render/DrawBasic/Shader.h"
#include "Utils/Hashing.h"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

namespace Rocket
{
    class VulkanShader : implements Shader
    {
    public:
        VulkanShader(const String& name) : m_NameId(AssetHashTable::HashString(name)) {}
        VulkanShader(const VulkanShader& rhs) = default;
        VulkanShader(VulkanShader&& rhs) = default;
        virtual ~VulkanShader() = default;

        void SetDevice(VkDevice& device) { m_DeviceHandle = device; }
        void SetIsBinary(bool type) { m_IsBinary = type; }

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
        bool m_IsBinary = false;;
        Vec<VkShaderModule> m_Shader;
        Vec<VkPipelineShaderStageCreateInfo> m_ShaderInfo;
        VkDevice m_DeviceHandle;
    };
}