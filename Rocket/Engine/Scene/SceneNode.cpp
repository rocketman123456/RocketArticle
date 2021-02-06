#include "Scene/SceneNode.h"

#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

SceneNode::SceneNode()
{
	auto id = newGuid();
	m_Id = std::hash<Guid>{}(id);
	m_Tag.TagStr = id.str();
}

SceneNode::SceneNode(const String& name)
{
	auto id = newGuid();
	m_Id = std::hash<Guid>{}(id);
	m_Tag.TagStr = name;
}

void SceneNode::SetParent(SceneNode& parent)
{
	m_Parent = &parent;
}

SceneNode* SceneNode::GetParent() const
{
	return m_Parent;
}

void SceneNode::AddChild(SceneNode& child)
{
	m_Children.push_back(&child);
}

const Vec<SceneNode*>& SceneNode::GetChildren() const
{
	return m_Children;
}

void SceneNode::SetComponent(SceneComponent& component)
{
	auto it = m_Components.find(component.GetType());

	if (it != m_Components.end())
	{
		it->second = &component;
	}
	else
	{
		m_Components.insert(std::make_pair(component.GetType(), &component));
	}
}

SceneComponent& SceneNode::GetComponent(const std::type_index index)
{
	return *m_Components.at(index);
}

bool SceneNode::HasComponent(const std::type_index index)
{
	return m_Components.count(index) > 0;
}
