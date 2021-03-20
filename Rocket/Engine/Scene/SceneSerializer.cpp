#include "Scene/SceneSerializer.h"
#include "Module/AssetLoader.h"
#include "Math/YamlMath.h"
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

    static void SerializeNode(YAML::Emitter& out, SceneNode& node)
    {
        out << YAML::BeginMap; // Entity
		out << YAML::Key << "SceneNode" << YAML::Value << node.GetId();

		out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const String& filepath)
    {
        YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << scene_->GetName();
		out << YAML::Key << "SceneNodes" << YAML::Value << YAML::BeginSeq;

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

		auto nodes = data["SceneNodes"];
		if (!nodes)
			return false;

		for (auto node : nodes)
		{
			uint64_t uuid = node["SceneNode"].as<uint64_t>();
		}

		return true;
    }

    bool SceneSerializer::DeserializeRuntime(const String& filepath)
    {
		RK_CORE_ASSERT(false, "Unfinish");
		return false;
    }
}