#include "Scene/Scene.h"
#include "Scene/SceneNode.h"

#include <queue>
#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

void Scene::OnUpdateRuntime(Timestep ts)
{
}

void Scene::OnUpdateEditor(Timestep ts)
{
}

void Scene::SetNodes(Vec<Scope<SceneNode>>&& nodes)
{
	RK_CORE_ASSERT(m_Nodes.empty(), "Scene nodes were already set");
	m_Nodes = std::move(nodes);
}

void Scene::AddNode(Scope<SceneNode>&& node)
{
	m_Nodes.emplace_back(std::move(node));
}

void Scene::AddChild(SceneNode& child)
{
	m_Root->AddChild(child);
}

void Scene::AddComponent(Scope<SceneComponent>&& component)
{
	if (component)
	{
		m_Components[component->GetType()].push_back(std::move(component));
	}
}

void Scene::AddComponent(Scope<SceneComponent>&& component, SceneNode& node)
{
	node.SetComponent(*component);

	if (component)
	{
		m_Components[component->GetType()].push_back(std::move(component));
	}
}

Scope<SceneComponent> Scene::GetModel(uint32_t index)
{
	//auto meshes = std::move(m_Components.at(typeid(SubMesh)));
	//return std::move(meshes.at(index));
	return Scope<SceneComponent>();
}

void Scene::SetComponents(const std::type_index& type_info, Vec<Scope<SceneComponent>>&& components)
{
	m_Components[type_info] = std::move(components);
}

const Vec<Scope<SceneComponent>>& Scene::GetComponents(const std::type_index& type_info) const
{
	return m_Components.at(type_info);
}

bool Scene::HasComponent(const std::type_index& type_info) const
{
	auto component = m_Components.find(type_info);
	return (component != m_Components.end() && !component->second.empty());
}

SceneNode* Scene::FindNode(const String& node_name)
{
	for (auto root_node : m_Root->GetChildren())
	{
		Queue<SceneNode*> traverse_nodes{};
		traverse_nodes.push(root_node);

		while (!traverse_nodes.empty())
		{
			auto node = traverse_nodes.front();
			traverse_nodes.pop();

			if (node->GetTagName() == node_name)
			{
				return node;
			}

			for (auto child_node : node->GetChildren())
			{
				traverse_nodes.push(child_node);
			}
		}
	}

	return nullptr;
}

void Scene::SetRootNode(SceneNode& node)
{
	m_Root = &node;
}

SceneNode& Scene::GetRootNode()
{
	return *m_Root;
}
