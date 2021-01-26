#include "Render/Shader.h"

namespace Rocket
{
    void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &shader)
	{
		RK_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	inline void ShaderLibrary::Add(const Ref<Shader> &shader)
	{
		auto &name = shader->GetName();
		Add(name, shader);
	}

	//Ref<Shader> ShaderLibrary::Load(const std::string &filepath)
	//{
	//	auto shader = Shader::Create(filepath);
	//	Add(shader);
	//	return shader;
	//}

	//Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath)
	//{
	//	auto shader = Shader::Create(filepath);
	//	Add(name, shader);
	//	return shader;
	//}

	Ref<Shader> ShaderLibrary::Get(const std::string &name)
	{
		RK_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string &name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}