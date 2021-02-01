#pragma once
#include "Render/DrawBasic/Shader.h"
#include "Utils/Hashing.h"

namespace Rocket
{
    using ShaderSourceList = Vec<std::pair<uint32_t, std::string>>;

    class OpenGLShader : implements Shader
    {
    public:
        OpenGLShader(const std::string& name) : m_NameId(AssetHashTable::HashString(name)) {}
        OpenGLShader(uint32_t shader, const std::string& name) : m_RendererId(shader), m_NameId(AssetHashTable::HashString(name)) {}
        OpenGLShader(const OpenGLShader& rhs) = default;
        OpenGLShader(OpenGLShader&& rhs) = default;
        virtual ~OpenGLShader() = default;

        bool Initialize(const ShaderSourceList& list) final;
        void Finalize() final;

        void Bind() const final;
		void Unbind() const final;

		bool SetInt32(const std::string& name, int32_t value) final;
        bool SetUInt32(const std::string& name, uint32_t value) final;
		bool SetInt32Array(const std::string& name, int32_t* values, uint32_t count) final;
        bool SetUInt32Array(const std::string& name, uint32_t* values, uint32_t count) final;

		bool SetFloat(const std::string& name, float value) final;
        bool SetDouble(const std::string& name, double value) final;

		bool SetVector2f(const std::string& name, const Vector2f& value) final;
		bool SetVector3f(const std::string& name, const Vector3f& value) final;
		bool SetVector4f(const std::string& name, const Vector4f& value) final;

        bool SetMatrix2f(const std::string& name, const Matrix2f& value) final;
		bool SetMatrix3f(const std::string& name, const Matrix3f& value) final;
		bool SetMatrix4f(const std::string& name, const Matrix4f& value) final;

        const std::string& GetName() const final { return AssetHashTable::GetStringFromId(m_NameId); }
        uint32_t GetRenderId() final { return m_RendererId; }
    private:
        uint32_t m_RendererId;
        uint64_t m_NameId;
    };
}