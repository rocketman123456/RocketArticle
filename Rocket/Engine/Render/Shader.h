#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"

namespace Rocket
{
    Interface Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual bool SetInt32(const std::string& name, int32_t value) = 0;
        virtual bool SetUInt32(const std::string& name, uint32_t value) = 0;
		virtual bool SetInt32Array(const std::string& name, int32_t* values, uint32_t count) = 0;
        virtual bool SetUInt32Array(const std::string& name, uint32_t* values, uint32_t count) = 0;

		virtual bool SetFloat(const std::string& name, float value) = 0;
        virtual bool SetDouble(const std::string& name, double value) = 0;

		virtual bool SetVector2f(const std::string& name, const Vector2f& value) = 0;
		virtual bool SetVector3f(const std::string& name, const Vector3f& value) = 0;
		virtual bool SetVector4f(const std::string& name, const Vector4f& value) = 0;

        virtual bool SetMatrix2f(const std::string& name, const Matrix2f& value) = 0;
		virtual bool SetMatrix3f(const std::string& name, const Matrix3f& value) = 0;
		virtual bool SetMatrix4f(const std::string& name, const Matrix3f& value) = 0;

        virtual const std::string& GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
    };

    inline std::ostream& operator<<(std::ostream& os, const Shader& s)
	{
		return os << s.ToString();
	}
}