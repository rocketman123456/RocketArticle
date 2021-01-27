#pragma once
#include "Render/Shader.h"

namespace Rocket
{
    class OpenGLShader : implements Shader
    {
    public:
        OpenGLShader(uint32_t shader, const std::string& name) : m_RendererID(shader), m_Name(name) {}
        virtual ~OpenGLShader() = default;

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
		bool SetMatrix4f(const std::string& name, const Matrix3f& value) final;

        const std::string& GetName() const { return m_Name; }

    private:
        uint32_t m_RendererID;
        std::string m_Name;
    };
}