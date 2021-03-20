#pragma once
#include "Core/Core.h"
#include "Math/GeomMath.h"

#define MAX_LIGHTS 100

namespace Rocket
{
    // TODO : change math to glm / other to avoid alignment issue
    // alignas(16)
    struct alignas(16) TextureId
    {
        intptr_t texture{-1};   // 8 bytes
        uint32_t width{0};      // 4 bytes
        uint32_t height{0};     // 4 bytes
        uint32_t depth{1};      // 4 bytes
        uint32_t index{0};      // 4 bytes
        float    mipmap{0.0f};  // 4 bytes
        uint8_t  padding[4];    // 4 bytes
    };                          // total 32 bytes

    struct alignas(16) MaterialTextures
    {
        TextureId diffuse_map;   // 32 bytes
        TextureId normal_map;    // 32 bytes
        TextureId metallic_map;  // 32 bytes
        TextureId roughness_map; // 32 bytes
        TextureId ao_map;        // 32 bytes
        TextureId height_map;    // 32 bytes
    };                          // total 192 bytes

    struct alignas(16) LightInfo
    {
        Matrix4f light_view_mat;                   // 64 bytes
        Matrix4f light_projection_mat;             // 64 bytes
        Vector4f light_position;                     // 16 bytes
        Vector4f light_color;                        // 16 bytes
        Vector4f light_direction;                    // 16 bytes
        Vector4f light_dist_atten_curve_params[2];      // 32 bytes
        Vector4f light_angle_atten_curve_params[2];     // 32 bytes
    };                                              // total 240 bytes

    struct alignas(16) PerFrameConstants
    {
        Matrix4f view_mat;        // 64 bytes
        Matrix4f projection_mat;  // 64 bytes
        Vector4f cam_position;            // 16 bytes
        int32_t  num_lights;         // 4 bytes
        int32_t  padding[3];        // 12 bytes
    };                              // total 160 bytes

    struct alignas(16) PerBatchConstants
    {
        Matrix4f model_matrix;  // 64 bytes
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
        int32_t frame_index = 0;
        DrawFrameContext frame_context;
        Vec<Ref<DrawBatchContext>> batch_contexts;
    };

    const size_t k_size_per_frame_constant_buffer = RK_ALIGN(sizeof(PerFrameConstants), 256);  // CB size is required to be 256-byte aligned.
    const size_t k_size_per_batch_constant_buffer = RK_ALIGN(sizeof(PerBatchConstants), 256);  // CB size is required to be 256-byte aligned.
}