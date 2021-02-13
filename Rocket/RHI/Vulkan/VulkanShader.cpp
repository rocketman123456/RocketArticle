#include "Vulkan/VulkanShader.h"

using namespace Rocket;

bool VullkanShader::Initialize(const ShaderSourceList& list)
{
    return true;
}

void VullkanShader::Finalize()
{
}

void VullkanShader::Bind() const
{
}

void VullkanShader::Unbind() const
{
}

bool VullkanShader::SetInt32(const String& name, int32_t value)
{
    return true;
}

bool VullkanShader::SetUInt32(const String& name, uint32_t value)
{
    return true;
}

bool VullkanShader::SetInt32Array(const String& name, int32_t* values, uint32_t count)
{
    return true;
}

bool VullkanShader::SetUInt32Array(const String& name, uint32_t* values, uint32_t count)
{
    return true;
}

bool VullkanShader::SetFloat(const String& name, float value)
{
    return true;
}

bool VullkanShader::SetDouble(const String& name, double value)
{
    return true;
}

bool VullkanShader::SetVector2f(const String& name, const Vector2f& value)
{
    return true;
}

bool VullkanShader::SetVector3f(const String& name, const Vector3f& value)
{
    return true;
}

bool VullkanShader::SetVector4f(const String& name, const Vector4f& value)
{
    return true;
}

bool VullkanShader::SetMatrix2f(const String& name, const Matrix2f& value)
{
    return true;
}

bool VullkanShader::SetMatrix3f(const String& name, const Matrix3f& value)
{
    return true;
}

bool VullkanShader::SetMatrix4f(const String& name, const Matrix4f& value)
{
    return true;
}