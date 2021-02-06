#include "Scene/SceneNode.h"

#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

SceneNode::SceneNode()
{
	auto id = newGuid();
	m_Id = std::hash<Guid>{}(id);
}

SceneNode::SceneNode(entt::entity handle, Scene* scene)
	: m_EntityHandle(handle), m_Scene(scene)
{
	auto id = newGuid();
	m_Id = std::hash<Guid>{}(id);
}
