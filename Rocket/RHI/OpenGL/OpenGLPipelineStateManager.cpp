#include "OpenGL/OpenGLPipelineStateManager.h"
#include "Module/Application.h"
#include "Module/AssetLoader.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <utility>

namespace Rocket
{
    PipelineStateManager* GetPipelineStateManager()
    {
        return new OpenGLPipelineStateManager();
    }

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

    using ShaderSourceList = Vec<std::pair<GLenum, std::string>>;

    static bool LoadShaderProgram(const ShaderSourceList& source, GLuint& shaderProgram)
    {
        int status;

        // Create a shader program object.
        shaderProgram = glCreateProgram();

        for (auto it = source.cbegin(); it != source.cend(); it++)
        {
            if (!it->second.empty()) {
                GLuint shader;
                auto config = g_Application->GetConfig();
                status = LoadShaderFromFile((config->GetAssetPath() + it->second).c_str(), it->first, shader);
                if (!status)
                {
                    return false;
                }

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

    bool OpenGLPipelineStateManager::InitializePipelineState(PipelineState** ppPipelineState)
    {
        bool result;
        OpenGLPipelineState* pnew_state =new OpenGLPipelineState(**ppPipelineState);

        ShaderSourceList list;

        if (!(*ppPipelineState)->vertexShaderName.empty()) {
            list.emplace_back(GL_VERTEX_SHADER, (*ppPipelineState)->vertexShaderName);
        }

        if (!(*ppPipelineState)->pixelShaderName.empty()) {
            list.emplace_back(GL_FRAGMENT_SHADER, (*ppPipelineState)->pixelShaderName);
        }

        if (!(*ppPipelineState)->geometryShaderName.empty()) {
            list.emplace_back(GL_GEOMETRY_SHADER, (*ppPipelineState)->geometryShaderName);
        }

        if (!(*ppPipelineState)->computeShaderName.empty()) {
            list.emplace_back(GL_COMPUTE_SHADER, (*ppPipelineState)->computeShaderName);
        }

        if (!(*ppPipelineState)->tessControlShaderName.empty()) {
            list.emplace_back(GL_TESS_CONTROL_SHADER, (*ppPipelineState)->tessControlShaderName);
        }

        if (!(*ppPipelineState)->tessEvaluateShaderName.empty()) {
            list.emplace_back(GL_TESS_EVALUATION_SHADER, (*ppPipelineState)->tessEvaluateShaderName);
        }

        result = LoadShaderProgram(list, pnew_state->shaderProgram);

        *ppPipelineState = pnew_state;

        return result;
    }

    void OpenGLPipelineStateManager::DestroyPipelineState(PipelineState& pipelineState)
    {
        OpenGLPipelineState* pPipelineState = dynamic_cast<OpenGLPipelineState*>(&pipelineState);
        glDeleteProgram(pPipelineState->shaderProgram);
    }
}