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
		String name;
		ShaderDataType type;
		uint32_t size = 0;
		size_t offset = 0;
		bool normalized = false;

		uint32_t index = 0;
		uint32_t binding = 0;
		uint32_t count = 0;
		uint32_t shader_stage = 0;
		uint32_t uniform_type = 0;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const String& name_, bool normalized = false)
			: name(name_), type(type), size(ShaderDataTypeSize(type)), normalized(normalized) {}
		BufferElement(const String& name_, uint32_t binding_, uint32_t count_, uint32_t stage, uint32_t type, bool normalize = false)
			: name(name_), binding(binding_), count(count_), shader_stage(stage), uniform_type(type), normalized(normalize) {}

		uint32_t GetComponentCount() const
		{
			switch (type)
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
			: elements(elements) { CalculateOffsetsAndStride(); }

		uint32_t GetStride() const { return stride; }
		const Vec<BufferElement>& GetElements() const { return elements; }
		void SetLayout(std::initializer_list<BufferElement> elements) { elements = elements; }

		Vec<BufferElement>::iterator begin() { return elements.begin(); }
		Vec<BufferElement>::iterator end() { return elements.end(); }
		Vec<BufferElement>::const_iterator begin() const { return elements.begin(); }
		Vec<BufferElement>::const_iterator end() const { return elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t index;
			size_t offset = 0;
			stride = 0;
			for (auto& element : elements)
			{
				element.index = index;
				element.offset = offset;
				index += 1;
				offset += element.size;
				stride += element.size;
			}
		}
	private:
		Vec<BufferElement> elements;
		uint32_t stride = 0;
	};
}