#pragma once
#include "Core/Core.h"

namespace Rocket
{
    enum class ShaderDataType : uint32_t
	{
		None = 0, Float, Vec2f, Vec3f, Vec4f, Mat2f, Mat3f, Mat4f, Int, Vec2i, Vec3i, Vec4i, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Vec2f:    return 4 * 2;
        case ShaderDataType::Vec3f:    return 4 * 3;
        case ShaderDataType::Vec4f:    return 4 * 4;
		case ShaderDataType::Mat2f:    return 4 * 2 * 2;
        case ShaderDataType::Mat3f:    return 4 * 3 * 3;
        case ShaderDataType::Mat4f:    return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Vec2i:    return 4 * 2;
        case ShaderDataType::Vec3i:    return 4 * 3;
        case ShaderDataType::Vec4i:    return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        default: RK_CORE_ASSERT(false, "Unknown ShaderDataType!");  return 0;
		}
	}

    struct BufferElement
	{
		String Name;
		ShaderDataType Type;
		uint32_t Size = 0;
		size_t Offset = 0;
		bool Normalized = false;

		uint32_t Index = 0;
		uint32_t Binding = 0;
		uint32_t Count = 0;
		uint32_t ShaderStage = 0;
		uint32_t UniformType = 0;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const String& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized) {}
		BufferElement(const String& name, uint32_t binding, uint32_t count, uint32_t stage, uint32_t type, bool normalized = false)
			: Name(name), Binding(binding), Count(count), ShaderStage(stage), UniformType(type), Normalized(normalized) {}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
            case ShaderDataType::Float:   return 1;
            case ShaderDataType::Vec2f:   return 2;
            case ShaderDataType::Vec3f:   return 3;
            case ShaderDataType::Vec4f:   return 4;
			case ShaderDataType::Mat2f:   return 3; // 2 * float2
            case ShaderDataType::Mat3f:   return 3; // 3 * float3
            case ShaderDataType::Mat4f:   return 4; // 4 * float4
            case ShaderDataType::Int:     return 1;
            case ShaderDataType::Vec2i:   return 2;
            case ShaderDataType::Vec3i:   return 3;
            case ShaderDataType::Vec4i:   return 4;
            case ShaderDataType::Bool:    return 1;
            default: RK_CORE_ASSERT(false, "Unknown ShaderDataType!");  return 0;
			}
		}
	};

    class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufferElement> elements)
			: m_Elements(elements) { CalculateOffsetsAndStride(); }

		uint32_t GetStride() const { return m_Stride; }
		const Vec<BufferElement>& GetElements() const { return m_Elements; }
		void SetLayout(std::initializer_list<BufferElement> elements) { m_Elements = elements; }

		Vec<BufferElement>::iterator begin() { return m_Elements.begin(); }
		Vec<BufferElement>::iterator end() { return m_Elements.end(); }
		Vec<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		Vec<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t index;
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Index = index;
				element.Offset = offset;
				index += 1;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		Vec<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}