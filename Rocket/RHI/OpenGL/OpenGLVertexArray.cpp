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
	case ShaderDataType::Mat2: return GL_FLOAT;
	case ShaderDataType::Mat3: return GL_FLOAT;
	case ShaderDataType::Mat4: return GL_FLOAT;
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
	glGenVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

inline void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

inline void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

inline void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	RK_GRAPHICS_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
		
	glBindVertexArray(m_RendererID);
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
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					count,
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(m_VertexBufferIndex, 1);
				m_VertexBufferIndex++;
			}
			break;
		}
		default:
			RK_CORE_ASSERT(false, "Unknown ShaderDataType!");
		}
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

inline void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}
