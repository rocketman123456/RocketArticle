#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"

#define MAX_LIGHTS 100

namespace Rocket
{
    struct alignas(16) TextureId
    {
        intptr_t texture{-1};   // 8 bytes
        uint32_t width{0};      // 4 bytes
        uint32_t height{0};     // 4 bytes
        uint32_t depth{1};      // 4 bytes
        uint32_t index{0};      // 4 bytes
        float    mipmap{0.0f};  // 4 bytes
        int32_t  padding;       // 4 bytes
    };                          // total 32 bytes

    struct alignas(16) MaterialTextures
    {
        TextureId diffuseMap;   // 32 bytes
        TextureId normalMap;    // 32 bytes
        TextureId metallicMap;  // 32 bytes
        TextureId roughnessMap; // 32 bytes
        TextureId aoMap;        // 32 bytes
        TextureId heightMap;    // 32 bytes
    };                          // total 192 bytes

    struct alignas(16) LightInfo
    {
        Matrix4f lightViewMatrix;                   // 64 bytes
        Matrix4f lightProjectionMatrix;             // 64 bytes
        Vector4f lightPosition;                     // 16 bytes
        Vector4f lightColor;                        // 16 bytes
        Vector4f lightDirection;                    // 16 bytes
        Vector4f lightDistAttenCurveParams[2];      // 32 bytes
        Vector4f lightAngleAttenCurveParams[2];     // 32 bytes
    };                                              // total 240 bytes

    struct alignas(16) PerFrameConstants
    {
        Matrix4f viewMatrix;        // 64 bytes
        Matrix4f projectionMatrix;  // 64 bytes
        Vector4f camPos;            // 16 bytes
        int32_t  numLights;         // 4 bytes
        int32_t  padding[3];        // 12 bytes
    };                              // total 160 bytes

    struct alignas(16) PerBatchConstants
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