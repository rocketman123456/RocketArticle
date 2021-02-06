#include "Scene/Scene.h"
#include "Scene/SceneNode.h"

#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

SceneNode Scene::CreateNode(const String& name)
{
	SceneNode entity = { m_Registry.create(), this };
	//entity.AddComponent<TransformComponent>();
	//auto& tag = entity.AddComponent<TagComponent>();
	//tag.Tag = name.empty() ? "Entity" : name;
	return entity;
}

void Scene::DestroyNode(SceneNode node)
{
	m_Registry.destroy(node);
}

void Scene::OnUpdateRuntime(Timestep ts)
{
}

void Scene::OnUpdateEditor(Timestep ts)
{
}
