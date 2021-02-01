#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"

#include <crossguid/guid.hpp>
#include <vector>

#define MAX_LIGHTS 100

namespace Rocket
{
    struct texture_id
    {
        intptr_t texture{-1};
        uint32_t width{0};
        uint32_t height{0};
        uint32_t depth{1};
        uint32_t index{0};
        float    mipmap{0.0f};
    };

    struct material_textures
    {
        texture_id diffuseMap;
        texture_id normalMap;
        texture_id metallicMap;
        texture_id roughnessMap;
        texture_id aoMap;
        texture_id heightMap;
    };

    struct PerFrameConstants
    {
        Matrix4f viewMatrix;        // 64 bytes
        Matrix4f projectionMatrix;  // 64 bytes
        Vector4f camPos;              // 16 bytes
    };

    struct PerBatchConstants
    {
        Matrix4f modelMatrix;  // 64 bytes
    };

    struct DrawFrameContext : PerFrameConstants
    {
        virtual ~DrawFrameContext() = default;
    };

    struct DrawBatchContext : PerBatchConstants
    {
        virtual ~DrawBatchContext() = default;
    };

    struct Frame
    {
        int32_t frameIndex = 0;
        DrawFrameContext frameContext;
        Vec<Ref<DrawBatchContext>> batchContexts;
    };

    const size_t kSizePerFrameConstantBuffer = RK_ALIGN(sizeof(PerFrameConstants), 256);  // CB size is required to be 256-byte aligned.
    const size_t kSizePerBatchConstantBuffer = RK_ALIGN(sizeof(PerBatchConstants), 256);  // CB size is required to be 256-byte aligned.
}