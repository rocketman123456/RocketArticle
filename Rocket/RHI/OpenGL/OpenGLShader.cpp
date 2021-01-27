#include "OpenGL/OpenGLShader.h"

#include <glad/glad.h>

namespace Rocket
{
    void OpenGLShader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

    bool OpenGLShader::SetInt32(const std::string& name, int32_t value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform1i(location, value);
        return true;
    }

    bool OpenGLShader::SetUInt32(const std::string& name, uint32_t value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform1ui(location, value);
        return true;
    }

    bool OpenGLShader::SetInt32Array(const std::string& name, int32_t* values, uint32_t count)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform1iv(location, count, values);
        return true;
    }

    bool OpenGLShader::SetUInt32Array(const std::string& name, uint32_t* values, uint32_t count)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform1uiv(location, count, values);
        return true;
    }

    bool OpenGLShader::SetFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform1f(location, value);
        return true;
    }

    bool OpenGLShader::SetDouble(const std::string& name, double value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform1d(location, value);
        return true;
    }

    bool OpenGLShader::SetVector2f(const std::string& name, const Vector2f& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform2f(location, value[0], value[1]);
        return true;
    }

    bool OpenGLShader::SetVector3f(const std::string& name, const Vector3f& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform3f(location, value[0], value[1], value[2]);
        return true;
    }

    bool OpenGLShader::SetVector4f(const std::string& name, const Vector4f& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniform4f(location, value[0], value[1], value[2], value[3]);
        return true;
    }

    bool OpenGLShader::SetMatrix2f(const std::string& name, const Matrix2f& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniformMatrix2fv(location, 1, GL_FALSE, (float*)value.data());
        return true;
    }

    bool OpenGLShader::SetMatrix3f(const std::string& name, const Matrix3f& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniformMatrix3fv(location, 1, GL_FALSE, (float*)value.data());
        return true;
    }

    bool OpenGLShader::SetMatrix4f(const std::string& name, const Matrix3f& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            return false;
        }
		glUniformMatrix4fv(location, 1, GL_FALSE, (float*)value.data());
        return true;
    }
}