#pragma once
#include "Render/DrawBasic/Shader.h"
#include "Utils/Hashing.h"

namespace Rocket
{
    class OpenGLShader : implements Shader
    {
    public:
        OpenGLShader(const String& name) : m_NameId(AssetHashTable::HashString(name)) {}
        OpenGLShader(uint32_t shader, const String& name) : m_RendererId(shader), m_NameId(AssetHashTable::HashString(name)) {}
        OpenGLShader(const OpenGLShader& rhs) = default;
        OpenGLShader(OpenGLShader&& rhs) = default;
        virtual ~OpenGLShader() = default;

        bool Initialize(const ShaderSourceList& list) final;
        void Finalize() final;

        void Bind() const final;
		void Unbind() const final;

        int32_t GetLocation(const String& name);

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
        uint32_t GetRenderId() final { return m_RendererId; }
    private:
        uint32_t m_RendererId;
        uint64_t m_NameId;
    };
}