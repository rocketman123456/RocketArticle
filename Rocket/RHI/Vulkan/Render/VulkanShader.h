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
        VulkanShader(const String& name) : name_id_(GlobalHashTable::HashString("Asset"_hash, name)) {}
        VulkanShader(const VulkanShader& rhs) = default;
        VulkanShader(VulkanShader&& rhs) = default;
        virtual ~VulkanShader() = default;

        void SetDevice(VkDevice& device) { device_handle_ = device; }
        void SetIsBinary(bool type) { is_binary_ = type; }

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

        const String& GetName() const final { return GlobalHashTable::GetStringFromId("Asset"_hash, name_id_); }
		uint32_t GetRenderId() const final { return renderer_id_; }

        Vec<VkShaderModule>& GetShader() { return shaders_; }
        Vec<VkPipelineShaderStageCreateInfo>& GetShaderInfo() { return shader_infos_; }
    private:
        uint64_t name_id_;
        uint32_t renderer_id_;
        bool is_binary_ = false;;
        Vec<VkShaderModule> shaders_;
        Vec<VkPipelineShaderStageCreateInfo> shader_infos_;
        VkDevice device_handle_;
    };
}