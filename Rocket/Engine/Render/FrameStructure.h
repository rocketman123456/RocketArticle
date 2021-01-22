#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"

#include <crossguid/guid.hpp>
#include <vector>

#define MAX_LIGHTS 100

namespace Rocket
{
    enum LightType { Omni = 0, Spot = 1, Infinity = 2, Area = 3 };
    ENUM(AttenCurveType) {kNone = 0, kLinear = 1, kSmooth = 2, kInverse = 3, kInverseSquare = 4};
    struct Light {
        Matrix4f lightViewMatrix;                 // 64 bytes
        Matrix4f lightProjectionMatrix;           // 64 bytes
        float lightIntensity;                     // 4 bytes
        LightType lightType;                      // 4 bytes
        int lightCastShadow;                      // 4 bytes
        int lightShadowMapIndex;                  // 4 bytes
        AttenCurveType lightAngleAttenCurveType;  // 4 bytes
        AttenCurveType lightDistAttenCurveType;   // 4 bytes
        Vector2f lightSize;                       // 8 bytes
        xg::Guid lightGuid;                       // 16 bytes
        Vector4f lightPosition;                   // 16 bytes
        Vector4f lightColor;                      // 16 bytes
        Vector4f lightDirection;                  // 16 bytes
        Vector4f lightDistAttenCurveParams[2];    // 32 bytes
        Vector4f lightAngleAttenCurveParams[2];   // 32 bytes
    };                                            // totle 288 bytes

    struct LightInfo 
    {
        struct Light lights[MAX_LIGHTS];  // 288 bytes * MAX_LIGHTS
    };

    struct DrawFrameContext
    {
        Matrix4f viewMatrix;        // 64 bytes
        Matrix4f projectionMatrix;  // 64 bytes
        Vector4f camPos;            // 16 bytes
        int32_t  numLights;         // 4 bytes
    };

    struct DrawBatchContext
    {
        Matrix4f modelMatrix;       // 64 bytes
    };

    struct Frame
    {
        int32_t frameIndex = 0;
        DrawFrameContext frameContext;
        std::vector<Ref<DrawBatchContext>> batchContexts;
        LightInfo lightInfo;
    };
}