#include "Render/DrawBasic/Texture.h"

namespace Rocket
{
    SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const Vector2f& min, const Vector2f& max)
        : m_Texture(texture)
    {
        m_TexCoords[0] = { min[0], min[1] };
        m_TexCoords[1] = { max[0], min[1] };
        m_TexCoords[2] = { max[0], max[1] };
        m_TexCoords[3] = { min[0], max[1] };
    }

    SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const Vector2f& coord, const Vector2f& cellSize, const Vector2f& spriteSize)
        : m_Texture(texture)
    {
        Vector2f min = { 
            (coord[0] * cellSize[0]) / texture->GetWidth(), 
            (coord[1] * cellSize[1]) / texture->GetHeight()
        };
        Vector2f max = { 
            ((coord[0] + spriteSize[0]) * cellSize[0]) / texture->GetWidth(),
            ((coord[1] + spriteSize[1]) * cellSize[1]) / texture->GetHeight()
        };
        m_TexCoords[0] = { min[0], min[1] };
        m_TexCoords[1] = { max[0], min[1] };
        m_TexCoords[2] = { max[0], max[1] };
        m_TexCoords[3] = { min[0], max[1] };
    }
}