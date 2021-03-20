#include "Scene/SceneNode.h"

#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

SceneNode::SceneNode() : transform_(*this)
{
	auto id = newGuid();
	id_ = std::hash<Guid>{}(id);
	tag_.tag_str = id.str();
}

SceneNode::SceneNode(const String& name) : transform_(*this)
{
	auto id = newGuid();
	id_ = std::hash<Guid>{}(id);
	tag_.tag_str = name;
}

void SceneNode::SetParent(SceneNode& parent)
{
	parent_ = &parent;
}

SceneNode* SceneNode::GetParent() const
{
	return parent_;
}

void SceneNode::AddChild(SceneNode& child)
{
	children_.push_back(&child);
}

const Vec<SceneNode*>& SceneNode::GetChildren() const
{
	return children_;
}

void SceneNode::SetComponent(SceneComponent& component)
{
	auto it = components_.find(component.GetType());

	if (it != components_.end())
	{
		it->second = &component;
	}
	else
	{
		components_.insert(std::make_pair(component.GetType(), &component));
	}
}

SceneComponent& SceneNode::GetComponent(const std::type_index index)
{
	return *components_.at(index);
}

bool SceneNode::HasComponent(const std::type_index index)
{
	return components_.count(index) > 0;
}
