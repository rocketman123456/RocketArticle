#include "Scene/Scene.h"
#include "Scene/SceneNode.h"

#include <queue>
#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	viewport_width_ = width;
	viewport_height_ = height;
}

void Scene::OnUpdateRuntime(Timestep ts)
{
}

void Scene::OnUpdateEditor(Timestep ts)
{
}

void Scene::SetNodes(Vec<Scope<SceneNode>>&& nodes)
{
	RK_CORE_ASSERT(nodes_.empty(), "Scene nodes were already set");
	nodes_ = std::move(nodes);
}

void Scene::AddNode(Scope<SceneNode>&& node)
{
	nodes_.emplace_back(std::move(node));
}

void Scene::AddChild(SceneNode& child)
{
	root_->AddChild(child);
}

void Scene::AddComponent(Scope<SceneComponent>&& component)
{
	if (component)
	{
		components_[component->GetType()].push_back(std::move(component));
	}
}

void Scene::AddComponent(Scope<SceneComponent>&& component, SceneNode& node)
{
	node.SetComponent(*component);

	if (component)
	{
		components_[component->GetType()].push_back(std::move(component));
	}
}

void Scene::SetComponents(const std::type_index& type_info, Vec<Scope<SceneComponent>>&& components)
{
	components_[type_info] = std::move(components);
}

const Vec<Scope<SceneComponent>>& Scene::GetComponents(const std::type_index& type_info) const
{
	return components_.at(type_info);
}

bool Scene::HasComponent(const std::type_index& type_info) const
{
	auto component = components_.find(type_info);
	return (component != components_.end() && !component->second.empty());
}

SceneNode* Scene::FindNode(const String& node_name)
{
	for (auto root_node : root_->GetChildren())
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
	root_ = &node;
}

SceneNode& Scene::GetRootNode()
{
	return *root_;
}
