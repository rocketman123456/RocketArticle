#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"
#include "Utils/Hashing.h"

namespace Rocket
{
	using ShaderSourceList = Vec<std::pair<uint32_t, String>>;

    Interface Shader
    {
    public:
        virtual ~Shader() = default;

		virtual bool Initialize(const ShaderSourceList& list) = 0;
		virtual void Finalize() = 0;

        virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual bool SetInt32(const String& name, int32_t value) = 0;
        virtual bool SetUInt32(const String& name, uint32_t value) = 0;
		virtual bool SetInt32Array(const String& name, int32_t* values, uint32_t count) = 0;
        virtual bool SetUInt32Array(const String& name, uint32_t* values, uint32_t count) = 0;

		virtual bool SetFloat(const String& name, float value) = 0;
        virtual bool SetDouble(const String& name, double value) = 0;

		virtual bool SetVector2f(const String& name, const Vector2f& value) = 0;
		virtual bool SetVector3f(const String& name, const Vector3f& value) = 0;
		virtual bool SetVector4f(const String& name, const Vector4f& value) = 0;

        virtual bool SetMatrix2f(const String& name, const Matrix2f& value) = 0;
		virtual bool SetMatrix3f(const String& name, const Matrix3f& value) = 0;
		virtual bool SetMatrix4f(const String& name, const Matrix4f& value) = 0;

        virtual const String& GetName() const = 0;
		virtual const String& ToString() const { return GetName(); }

		virtual uint32_t GetRenderId() const = 0;
    };

    inline std::ostream& operator<<(std::ostream& os, const Shader& s)
	{
		return os << s.ToString();
	}
}