#include "Scene/SceneSerializer.h"
#include "Module/AssetLoader.h"
#include "Utils/YamlMath.h"
#include "Utils/Hashing.h"

namespace Rocket
{
    YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2f& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v[0] << v[1] << YAML::EndSeq;
		return out;
	}

    YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3f& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v[0] << v[1] << v[2] << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4f& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v[0] << v[1] << v[2] << v[3] << YAML::EndSeq;
		return out;
	}

    static void SerializeEntity(YAML::Emitter& out, Entity& entity)
    {
        //uint32_t id = entity;
        out << YAML::BeginMap; // Entity
		//out << YAML::Key << "Entity" << YAML::Value << HashFunction::Hash<uint32_t>(id);

		out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const String& filepath)
    {
        YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->GetName();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		out << YAML::EndSeq;
		out << YAML::EndMap;

        g_AssetLoader->SyncOpenAndWriteStringToTextFile(filepath, out.c_str());
    }

    void SceneSerializer::SerializeRuntime(const String& filepath)
    {
		RK_CORE_ASSERT(false, "Unfinish");
    }

    bool SceneSerializer::Deserialize(const String& filepath)
    {
		YAML::Node data = YAML::LoadFile(filepath);
		if (!data["Scene"])
			return false;

		String sceneName = data["Scene"].as<String>();
		RK_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities)
			return false;

		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();
		}

		return true;
    }

    bool SceneSerializer::DeserializeRuntime(const String& filepath)
    {
		RK_CORE_ASSERT(false, "Unfinish");
		return false;
    }
}