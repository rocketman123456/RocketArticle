#include "OpenGL/OpenGLPipelineStateManager.h"

#include <glad/glad.h>
#include <fstream>
#include <utility>

namespace Rocket
{
    static void OutputShaderErrorMessage(unsigned int shaderId, const char* shaderFilename) 
    {
        int logSize, i;
        char* infoLog;
        std::ofstream fout;

        // Get the size of the string containing the information log for the failed
        // shader compilation message.
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

        // Increment the size by one to handle also the null terminator.
        logSize++;

        // Create a char buffer to hold the info log.
        infoLog = new char[logSize];
        if (!infoLog) {
            return;
        }

        // Now retrieve the info log.
        glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

        // Open a file to write the error message to.
        fout.open("shader-error.txt");

        // Write out the error message.
        for (i = 0; i < logSize; i++) {
            fout << infoLog[i];
        }

        // Close the file.
        fout.close();

        // Pop a message up on the screen to notify the user to check the text file
        // for compile errors.
        RK_GRAPHICS_ERROR(infoLog);
        RK_GRAPHICS_ERROR("Error compiling shader : {}.  Check shader-error.txt for message.", shaderFilename);

        return;
    }

    static void OutputLinkerErrorMessage(unsigned int programId)
    {
        int logSize, i;
        char* infoLog;
        std::ofstream fout;

        // Get the size of the string containing the information log for the failed
        // shader compilation message.
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

        // Increment the size by one to handle also the null terminator.
        logSize++;

        // Create a char buffer to hold the info log.
        infoLog = new char[logSize];
        if (!infoLog) {
            return;
        }

        // Now retrieve the info log.
        glGetProgramInfoLog(programId, logSize, NULL, infoLog);

        // Open a file to write the error message to.
        fout.open("linker-error.txt");

        // Write out the error message.
        for (i = 0; i < logSize; i++) {
            fout << infoLog[i];
        }

        // Close the file.
        fout.close();

        // Pop a message up on the screen to notify the user to check the text file
        // for linker errors.
        RK_GRAPHICS_ERROR(infoLog);
        RK_GRAPHICS_ERROR("Error compiling linker.  Check linker-error.txt for message.");
    }

    static bool LoadShaderFromFile(const char* filename, const GLenum shaderType, GLuint& shader)
    {
        std::string cbufferShaderBuffer;
        std::string commonShaderBuffer;
        std::string shaderBuffer;
        int status;
    }

    using ShaderSourceList = Vec<std::pair<GLenum, std::string>>;

    bool OpenGLPipelineStateManagerCommonBase::InitializePipelineState(PipelineState** ppPipelineState)
    {
        bool result;
        OpenGLPipelineState* pnew_state =new OpenGLPipelineState(**ppPipelineState);

        ShaderSourceList list;
    }

    void OpenGLPipelineStateManagerCommonBase::DestroyPipelineState(PipelineState& pipelineState)
    {
        OpenGLPipelineState* pPipelineState = dynamic_cast<OpenGLPipelineState*>(&pipelineState);
        glDeleteProgram(pPipelineState->shaderProgram);
    }
}