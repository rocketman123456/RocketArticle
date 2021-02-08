#pragma once
#include "Core/Core.h"

namespace Rocket
{
    ENUM(DRAW_TYPE) { STATIC = 0, DYNAMIC };

    Interface UniformBuffer
    {
    public:
        //UniformBuffer() = default;
        virtual ~UniformBuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void SetData(void* data) = 0;
        virtual void SetSubData(void* data, uint32_t start, uint32_t size) = 0;

        virtual uint32_t GetRenderID() const = 0;
    };
}