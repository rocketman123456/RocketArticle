#include "OpenGL/OpenGLShader.h"
#include "Module/AssetLoader.h"
#include "Module/Application.h"

#include <glad/glad.h>
#include <fstream>

using namespace Rocket;

static void OutputShaderErrorMessage(unsigned int shaderId, const String& shaderFilename) 
{
    int logSize, i;
    char* infoLog = nullptr;

    // Get the size of the string containing the information log for the failed
    // shader compilation message.
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

    if(logSize > 1)
    {
        // Increment the size by one to handle also the null terminator.
        logSize++;

        // Create a char buffer to hold the info log.
        infoLog = new char[logSize];
        if (!infoLog)
            return;

        // Now retrieve the info log.
        glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

        // Write out the error message.
        g_AssetLoader->SyncOpenAndWriteStringToTextFile("shader-error.txt", infoLog);

        // Pop a message up on the screen to notify the user to check the text file
        // for compile errors.
        RK_GRAPHICS_ERROR(infoLog);
        RK_GRAPHICS_ERROR("Error compiling shader : {}.  Check shader-error.txt for message.", shaderFilename);
        delete[] infoLog;
    }
}

static void OutputLinkerErrorMessage(unsigned int programId)
{
    int logSize, i;
    char* infoLog = nullptr;

    // Get the size of the string containing the information log for the failed
    // shader compilation message.
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

    if(logSize > 1)
    {
        // Increment the size by one to handle also the null terminator.
        logSize++;

        // Create a char buffer to hold the info log.
        infoLog = new char[logSize];
        if (!infoLog)
            return;

        // Now retrieve the info log.
        glGetProgramInfoLog(programId, logSize, NULL, infoLog);

        // Write out the error message.
        g_AssetLoader->SyncOpenAndWriteStringToTextFile("linker-error.txt", infoLog);

        // Pop a message up on the screen to notify the user to check the text file
        // for linker errors.
        RK_GRAPHICS_ERROR(infoLog);
        RK_GRAPHICS_ERROR("Error compiling linker.  Check linker-error.txt for message.");
        delete[] infoLog;
    }
}

static bool LoadShaderFromFile(const String& filename, const GLenum shaderType, GLuint& shader)
{
    String cbufferShaderBuffer;
    String commonShaderBuffer;
    String shaderBuffer;
    int status;

    // Load the shader source file into a text buffer.
    shaderBuffer = g_AssetLoader->SyncOpenAndReadTextFileToString(filename);
    //shaderBuffer = cbufferShaderBuffer + commonShaderBuffer + shaderBuffer;

    // Create a shader object.
    shader = glCreateShader(shaderType);

    // Copy the shader source code strings into the shader objects.
    const char* pStr = shaderBuffer.c_str();
    glShaderSource(shader, 1, &pStr, NULL);

    // Compile the shaders.
    glCompileShader(shader);

    // Check to see if the shader compiled successfully.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != 1)
    {
        // If it did not compile then write the syntax error message out to a
        // text file for review.
        OutputShaderErrorMessage(shader, filename);
        return false;
    }

    return true;
}

static bool LoadShaderProgram(const ShaderSourceList& source, GLuint& shaderProgram)
{
    int status;

    // Create a shader program object.
    shaderProgram = glCreateProgram();

    for (auto it = source.cbegin(); it != source.cend(); it++)
    {
        if (!it->second.empty())
        {
            GLuint shader;
            auto config = g_Application->GetConfig();
            auto full_path = ("Shaders/" + RenderAPI + "/" + it->second);
            RK_GRAPHICS_INFO("Load Shader File {}", full_path);
            status = LoadShaderFromFile(full_path, it->first, shader);
            if (!status)
                return false;

            // Attach the shader to the program object.
            glAttachShader(shaderProgram, shader);
            glDeleteShader(shader);
        }
    }

    // Link the shader program.
    glLinkProgram(shaderProgram);

    // Check the status of the link.
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status != 1)
    {
        // If it did not link then write the syntax error message out to a text
        // file for review.
        OutputLinkerErrorMessage(shaderProgram);
        return false;
    }

    return true;
}

bool OpenGLShader::Initialize(const ShaderSourceList& list)
{
    bool result = LoadShaderProgram(list, m_RendererId);
    return result;
}

void OpenGLShader::Finalize()
{
    glDeleteProgram(m_RendererId);
}

void OpenGLShader::Bind() const
{
    glUseProgram(m_RendererId);
}

void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

int32_t OpenGLShader::GetLocation(const String& name)
{
    int32_t location = glGetUniformLocation(m_RendererId, name.c_str());
#if defined(RK_DEBUG)
    if (location == -1)
    {
        RK_CORE_ASSERT(false, "Get Shader Uniform Location Error")
    }
#endif
    return location;
}

bool OpenGLShader::SetInt32(const String &name, int32_t value)
{
    GLint location = GetLocation(name);
    glUniform1i(location, value);
    return true;
}

bool OpenGLShader::SetUInt32(const String &name, uint32_t value)
{
    GLint location = GetLocation(name);
    glUniform1ui(location, value);
    return true;
}

bool OpenGLShader::SetInt32Array(const String &name, int32_t *values, uint32_t count)
{
    GLint location = GetLocation(name);
    glUniform1iv(location, count, values);
    return true;
}

bool OpenGLShader::SetUInt32Array(const String &name, uint32_t *values, uint32_t count)
{
    GLint location = GetLocation(name);
    glUniform1uiv(location, count, values);
    return true;
}

bool OpenGLShader::SetFloat(const String &name, float value)
{
    GLint location = GetLocation(name);
    glUniform1f(location, value);
    return true;
}

bool OpenGLShader::SetDouble(const String &name, double value)
{
    GLint location = GetLocation(name);
    glUniform1d(location, value);
    return true;
}

bool OpenGLShader::SetVector2f(const String &name, const Vector2f &value)
{
    GLint location = GetLocation(name);
    glUniform2f(location, value[0], value[1]);
    return true;
}

bool OpenGLShader::SetVector3f(const String &name, const Vector3f &value)
{
    GLint location = GetLocation(name);
    glUniform3f(location, value[0], value[1], value[2]);
    return true;
}

bool OpenGLShader::SetVector4f(const String &name, const Vector4f &value)
{
    GLint location = GetLocation(name);
    glUniform4f(location, value[0], value[1], value[2], value[3]);
    return true;
}

bool OpenGLShader::SetMatrix2f(const String &name, const Matrix2f &value)
{
    GLint location = GetLocation(name);
    glUniformMatrix2fv(location, 1, GL_FALSE, value.data());
    return true;
}

bool OpenGLShader::SetMatrix3f(const String &name, const Matrix3f &value)
{
    GLint location = GetLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, value.data());
    return true;
}

bool OpenGLShader::SetMatrix4f(const String &name, const Matrix4f &value)
{
    GLint location = GetLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
    return true;
}
