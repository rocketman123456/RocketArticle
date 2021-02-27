#include "OpenGL/OpenGLUniformBuffer.h"

#include <glad/glad.h>

using namespace Rocket;

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, DRAW_TYPE type)
{
    m_BufferSize = size;
    m_Type = type;
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    switch(m_Type)
    {
        case DRAW_TYPE::STATIC:
        glBufferData(GL_UNIFORM_BUFFER, m_BufferSize, NULL, GL_STATIC_DRAW); // 分配size字节的内存
        break;
        case DRAW_TYPE::DYNAMIC:
        glBufferData(GL_UNIFORM_BUFFER, m_BufferSize, NULL, GL_DYNAMIC_DRAW); // 分配size字节的内存
        break;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void OpenGLUniformBuffer::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
}

void OpenGLUniformBuffer::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::SetData(void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    switch(m_Type)
    {
    case DRAW_TYPE::STATIC:
        glBufferData(GL_UNIFORM_BUFFER, m_BufferSize, data, GL_STATIC_DRAW); // 分配size字节的内存
        break;
    case DRAW_TYPE::DYNAMIC:
        glBufferData(GL_UNIFORM_BUFFER, m_BufferSize, data, GL_DYNAMIC_DRAW); // 分配size字节的内存
        break;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::SetSubData(void* data, uint32_t start, uint32_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, start, size, data); 
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::BindShader(uint32_t shader_id, uint32_t block_index, uint32_t pos)
{
    glUniformBlockBinding(shader_id, block_index, pos);
    glBindBufferBase(GL_UNIFORM_BUFFER, pos, m_RendererID);
}
