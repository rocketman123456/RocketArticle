#pragma once
#include "Core/Core.h"
#include "Graph/GraphNode.h"

#include <json.hpp>

namespace Rocket
{
	struct GraphEdge
	{
		size_t id;
		size_t from;
		size_t to;
		nlohmann::json options;
		GraphEdge(size_t i, size_t s, size_t t) : id(i), from(s), to(t) {};
	};

	class Graph
	{
	public:
		Graph(const String& name) : name_(name) {}

		static const size_t node_not_found = 0;

		/**
		 * @brief Create a new style
		 *
		 * @param style_name the group name
		 * @param color the hex color of the group
		 */
		void NewStyle(String style_name, String color);

		/**
		 * @brief Create a node object
		 *
		 * @param title of node
		 * @param style corresponds to the key used when using new_type(style, color)
		 * @param data json data to be displayed with node
		 * @return size_t id of node
		 */
		size_t CreateNode(const String& title = "Node", const String& style = NULL, const nlohmann::json& data = {});

		/**
		 * @brief Find a node from a reference
		 * 		  If the node does not exist then the reference will be node_not_found
		 *
		 * @param name of node
		 * @return size_t if of node
		 */
		size_t FindRef(String& name);

		/**
		 * @brief Add a readable reference to a node
		 *
		 * @param name of reference
		 * @param id of node
		 */
		void AddRef(String& name, size_t id);

		/**
		 * @brief Remove a readable reference to a node
		 *
		 * @param name of ref
		 */
		void RemoveRef(String& name);

		/**
		 * @brief Add an edge to the graph
		 * @param from source node
		 * @param to target node
		 */
		void AddEdge(size_t from, size_t to);

		/**
		 * @brief Remove Edge from the graph
		 * @param from source node
		 * @param to target node
		 */
		void RemoveEdge(size_t from, size_t to);

		/**
		 * @brief Dump the graphs state to json in the given file name
		 * @param file_name to dump to
		 */
		bool DumpToFile(String file_name);

		size_t NewId();

	private:
		size_t							next_id_ = 1;
		Vec<GraphEdge>                  adj_;
		UMap<size_t, Scope<GraphNode>>	nodes_;
		UMap<String, size_t>			refs_;
		String                          name_;
		UMap<String, String>			style_colors_;
	};
}
