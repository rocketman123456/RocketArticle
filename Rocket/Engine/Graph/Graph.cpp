#include "Graph/Graph.h"

using namespace Rocket;

void Graph::NewStyle(String style_name, String color)
{
	auto it = m_StyleColors.find(style_name);
	if (it != m_StyleColors.end())
	{
		it->second = color;
	}
	else
	{
		m_StyleColors.insert({ style_name, color });
	}
}

size_t Graph::CreateNode(const String& title, const String& style, const nlohmann::json& data)
{
	size_t id = NewId();
	m_Nodes[id] = CreateScope<GraphNode>(id, title, style, data);
	return id;
}

size_t Graph::NewId()
{
	return m_NextId++;
}

size_t Graph::FindRef(String& name)
{
	auto it = m_Refs.find(name);
	if (it == m_Refs.end())
	{
		return node_not_found;
	}
	return it->second;
}

void Graph::AddRef(String& name, size_t id)
{
	m_Refs.insert({ name, id });
}

void Graph::RemoveRef(String& name)
{
	auto it = m_Refs.find(name);
	if (it != m_Refs.end())
	{
		m_Refs.erase(it);
	}
}

void Graph::AddEdge(size_t from, size_t to)
{
	auto it = std::find_if(m_Adj.begin(), m_Adj.end(), [from, to](auto& e) -> bool { return e.From == from && e.To == to; });
	if (it == m_Adj.end())
	{
		m_Adj.push_back({ NewId(), from, to });
	}
}

void Graph::RemoveEdge(size_t from, size_t to)
{
	auto it = std::find_if(m_Adj.begin(), m_Adj.end(), [from, to](auto& e) -> bool { return e.From == from && e.To == to; });
	if (it != m_Adj.end())
	{
		m_Adj.erase(it);
	}
}

bool Graph::DumpToFile(String file)
{
	std::vector<nlohmann::json> edges;
	for (auto& e : m_Adj)
	{
		auto it = m_Nodes.find(e.From);
		if (it != m_Nodes.end())
		{
			e.options["style"] = it->second->attributes["style"];
		}
		e.options["id"] = e.Id;
		e.options["source"] = e.From;
		e.options["target"] = e.To;
		edges.push_back({ {"data", e.options} });
	}

	Vec<nlohmann::json> node_json;
	auto it = m_Nodes.begin();
	while (it != m_Nodes.end())
	{
		node_json.push_back(it->second->attributes);
		it++;
	}

	nlohmann::json j = {
		{"name", m_Name},
		{"nodes", node_json},
		{"edges", edges},
		{"styles", m_StyleColors}
	};

	// TODO : Iplements Write To Json File
	//fs::write_json(j, file);
	return true;
}
