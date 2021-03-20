#include "OpenGL/OpenGLVertexArray.h"

#include <glad/glad.h>

using namespace Rocket;

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float: return GL_FLOAT;
	case ShaderDataType::Vec2f: return GL_FLOAT;
	case ShaderDataType::Vec3f: return GL_FLOAT;
	case ShaderDataType::Vec4f: return GL_FLOAT;
	case ShaderDataType::Mat2f: return GL_FLOAT;
	case ShaderDataType::Mat3f: return GL_FLOAT;
	case ShaderDataType::Mat4f: return GL_FLOAT;
	case ShaderDataType::Int: return GL_INT;
	case ShaderDataType::Vec2i: return GL_INT;
	case ShaderDataType::Vec3i: return GL_INT;
	case ShaderDataType::Vec4i: return GL_INT;
	case ShaderDataType::Bool: return GL_BOOL;
	default: RK_CORE_ASSERT(false, "Unknown ShaderDataType!"); return 0;
	}
}

OpenGLVertexArray::OpenGLVertexArray()
{
	glGenVertexArrays(1, &renderer_id_);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &renderer_id_);
}

inline void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(renderer_id_);
}

inline void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

inline void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	RK_GRAPHICS_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

	glBindVertexArray(renderer_id_);
	vertexBuffer->Bind();

	const auto& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout)
	{
		switch (element.Type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Vec2f:
		case ShaderDataType::Vec3f:
		case ShaderDataType::Vec4f:
		case ShaderDataType::Int:
		case ShaderDataType::Vec2i:
		case ShaderDataType::Vec3i:
		case ShaderDataType::Vec4i:
		case ShaderDataType::Bool:
		{
			glEnableVertexAttribArray(vertex_buffer_index_);
			glVertexAttribPointer(vertex_buffer_index_,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			vertex_buffer_index_++;
			break;
		}
		case ShaderDataType::Mat2f:
		case ShaderDataType::Mat3f:
		case ShaderDataType::Mat4f:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(vertex_buffer_index_);
				glVertexAttribPointer(vertex_buffer_index_,
					count,
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(vertex_buffer_index_, 1);
				vertex_buffer_index_++;
			}
			break;
		}
		default:
			RK_CORE_ASSERT(false, "Unknown ShaderDataType!");
		}
	}

	vertex_buffers_.push_back(vertexBuffer);
	glBindVertexArray(0);
}

inline void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(renderer_id_);
	indexBuffer->Bind();
	index_buffer_ = indexBuffer;
	glBindVertexArray(0);
}
