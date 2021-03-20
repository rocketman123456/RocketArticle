#include "Graph/Graph.h"

using namespace Rocket;

void Graph::NewStyle(String style_name, String color)
{
	auto it = style_colors_.find(style_name);
	if (it != style_colors_.end())
	{
		it->second = color;
	}
	else
	{
		style_colors_.insert({ style_name, color });
	}
}

size_t Graph::CreateNode(const String& title, const String& style, const nlohmann::json& data)
{
	size_t id = NewId();
	nodes_[id] = CreateScope<GraphNode>(id, title, style, data);
	return id;
}

size_t Graph::NewId()
{
	return next_id_++;
}

size_t Graph::FindRef(String& name)
{
	auto it = refs_.find(name);
	if (it == refs_.end())
	{
		return node_not_found;
	}
	return it->second;
}

void Graph::AddRef(String& name, size_t id)
{
	refs_.insert({ name, id });
}

void Graph::RemoveRef(String& name)
{
	auto it = refs_.find(name);
	if (it != refs_.end())
	{
		refs_.erase(it);
	}
}

void Graph::AddEdge(size_t from, size_t to)
{
	auto it = std::find_if(adj_.begin(), adj_.end(), [from, to](auto& e) -> bool { return e.from == from && e.to == to; });
	if (it == adj_.end())
	{
		adj_.push_back({ NewId(), from, to });
	}
}

void Graph::RemoveEdge(size_t from, size_t to)
{
	auto it = std::find_if(adj_.begin(), adj_.end(), [from, to](auto& e) -> bool { return e.from == from && e.to == to; });
	if (it != adj_.end())
	{
		adj_.erase(it);
	}
}

bool Graph::DumpToFile(String file)
{
	std::vector<nlohmann::json> edges;
	for (auto& e : adj_)
	{
		auto it = nodes_.find(e.from);
		if (it != nodes_.end())
		{
			e.options["style"] = it->second->attributes["style"];
		}
		e.options["id"] = e.id;
		e.options["source"] = e.from;
		e.options["target"] = e.to;
		edges.push_back({ {"data", e.options} });
	}

	Vec<nlohmann::json> node_json;
	auto it = nodes_.begin();
	while (it != nodes_.end())
	{
		node_json.push_back(it->second->attributes);
		it++;
	}

	nlohmann::json j = {
		{"name", name_},
		{"nodes", node_json},
		{"edges", edges},
		{"styles", style_colors_}
	};

	// TODO : Iplements Write To Json File
	//fs::write_json(j, file);
	return true;
}
