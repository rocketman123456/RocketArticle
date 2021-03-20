#include "OpenGL/OpenGLUniformBuffer.h"

#include <glad/glad.h>

using namespace Rocket;

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, DRAW_TYPE type)
{
    buffer_size_ = size;
    type_ = type;
    glGenBuffers(1, &renderer_id_);
    glBindBuffer(GL_UNIFORM_BUFFER, renderer_id_);
    switch(type_)
    {
        case DRAW_TYPE::STATIC:
        glBufferData(GL_UNIFORM_BUFFER, buffer_size_, NULL, GL_STATIC_DRAW); // 分配size字节的内存
        break;
        case DRAW_TYPE::DYNAMIC:
        glBufferData(GL_UNIFORM_BUFFER, buffer_size_, NULL, GL_DYNAMIC_DRAW); // 分配size字节的内存
        break;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &renderer_id_);
}

void OpenGLUniformBuffer::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, renderer_id_);
}

void OpenGLUniformBuffer::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::SetData(void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, renderer_id_);
    switch(type_)
    {
    case DRAW_TYPE::STATIC:
        glBufferData(GL_UNIFORM_BUFFER, buffer_size_, data, GL_STATIC_DRAW); // 分配size字节的内存
        break;
    case DRAW_TYPE::DYNAMIC:
        glBufferData(GL_UNIFORM_BUFFER, buffer_size_, data, GL_DYNAMIC_DRAW); // 分配size字节的内存
        break;
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::SetSubData(void* data, uint32_t start, uint32_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, renderer_id_);
    glBufferSubData(GL_UNIFORM_BUFFER, start, size, data); 
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::BindShader(uint32_t shader_id, uint32_t block_index, uint32_t pos)
{
    glUniformBlockBinding(shader_id, block_index, pos);
    glBindBufferBase(GL_UNIFORM_BUFFER, pos, renderer_id_);
}
