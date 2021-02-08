#pragma once
#include "Render/DrawBasic/UniformBuffer.h"

namespace Rocket
{
    class OpenGLUniformBuffer : implements UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, DRAW_TYPE type);
        virtual ~OpenGLUniformBuffer();

        void Bind() final;
        void Unbind() final;
        void SetData(void* data) final;
        void SetSubData(void* data, uint32_t start, uint32_t size) final;

        uint32_t GetRenderID() const final { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        uint32_t m_BufferSize;
        DRAW_TYPE m_Type;
    };
}