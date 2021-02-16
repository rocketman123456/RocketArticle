#pragma once
#include "Render/DrawBasic/Texture.h"

namespace Rocket
{
    class VulkanTexture2D : implements Texture2D
    {
    public:
        VulkanTexture2D(uint32_t width, uint32_t height);
        VulkanTexture2D(String path);
        virtual ~VulkanTexture2D();

        uint32_t GetWidth() const final { return m_Width; }
		uint32_t GetHeight() const final { return m_Height; }
		uint32_t GetRendererID() const final { return m_RendererID; }

		void SetData(void* data, uint32_t size) final;

		void Bind(uint32_t slot = 0) const final;
		void Unbind(uint32_t slot = 0) const final;

		bool operator==(const Texture& other) const final
        {
            return m_RendererID == ((VulkanTexture2D&)other).m_RendererID;
        }

    private:
        uint32_t m_RendererID;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_MipLevels;
    };
}