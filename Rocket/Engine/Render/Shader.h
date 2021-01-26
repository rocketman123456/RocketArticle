#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"

namespace Rocket
{
    Interface Shader
    {
    public:
        Shader();
        virtual ~Shader();

        virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt32(const std::string& name, int32_t value) = 0;
        virtual void SetUInt32(const std::string& name, uint32_t value) = 0;
		virtual void SetInt32Array(const std::string& name, int32_t* values, uint32_t count) = 0;
        virtual void SetUInt32Array(const std::string& name, uint32_t* values, uint32_t count) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetDouble(const std::string& name, double value) = 0;

		virtual void SetVector2f(const std::string& name, const Vector2f& value) = 0;
		virtual void SetVector3f(const std::string& name, const Vector3f& value) = 0;
		virtual void SetVector4f(const std::string& name, const Vector4f& value) = 0;

        virtual void SetMatrix2f(const std::string& name, const Matrix2f& value) = 0;
		virtual void SetMatrix3f(const std::string& name, const Matrix3f& value) = 0;
		virtual void SetMatrix4f(const std::string& name, const Matrix3f& value) = 0;

        virtual const std::string& GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
    };

    inline std::ostream& operator<<(std::ostream& os, const Shader& s)
	{
		return os << s.ToString();
	}

    class ShaderLibrary
	{
	public:
		void Add(const std::string &name, const Ref<Shader> &shader);
		inline void Add(const Ref<Shader> &shader);

		//Ref<Shader> Load(const std::string &filepath);
		//Ref<Shader> Load(const std::string &name, const std::string &filepath);

		Ref<Shader> Get(const std::string &name);
		bool Exists(const std::string &name) const;

		static Ref<ShaderLibrary> Create() { return Ref<ShaderLibrary>(new ShaderLibrary()); }

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}