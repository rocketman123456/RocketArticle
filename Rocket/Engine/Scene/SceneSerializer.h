#pragma once
#include "Scene/Scene.h"

namespace Rocket
{
	// TODO : implements SceneSerializer
    class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}

		void Serialize(const String& filepath);
		void SerializeRuntime(const String& filepath);

		bool Deserialize(const String& filepath);
		bool DeserializeRuntime(const String& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}