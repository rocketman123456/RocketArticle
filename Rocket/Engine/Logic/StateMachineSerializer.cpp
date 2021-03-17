#include "Logic/StateMachineSerializer.h"
#include "Module/AssetLoader.h"
#include "Utils/Hashing.h"
#include "Common/GeomMath.h"

using namespace Rocket;

// TODO : make this as function input
// TODO : use struct to warp it
UMap<uint64_t, uint64_t> node_2_mat;
//UMap<uint64_t, uint64_t> edge_2_mat;
UMap<uint64_t, uint64_t> action_2_mat;
Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic> transfer_edge_mat;

Ref<StateMachine> StateMachineSerializer::Deserialize(const String& name, TransferFunction transfer, ActionFunction action)
{
    String asset_path = g_AssetLoader->GetAssetPath();
    String full_path = asset_path + name;

    YAML::Node root = YAML::LoadFile(full_path);

    UMap<String, Ref<StateNode>> node_map;
    UMap<String, Ref<StateEdge>> edge_map;
    UMap<String, uint64_t> action_map;
    UMap<uint64_t, std::pair<uint64_t, uint64_t>> action_node_pair;

    uint64_t action_count = 0;
    uint64_t node_count = 0;
    uint64_t edge_count = 0;

    String init_name = root["init_node"].as<String>();

    node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, init_name)] = node_count;
    node_count++;

    auto it_yaml = root["edge"].begin();
    while(it_yaml != root["edge"].end())
    {
        String edge_name = (*it_yaml)["name"].as<String>();
        String parent_name = (*it_yaml)["parent"].as<String>();
        String child_name = (*it_yaml)["child"].as<String>();
        String action_name = (*it_yaml)["action"].as<String>();

        RK_CORE_TRACE("Edge : edge {}, parent {}, child {}, action {}", edge_name, parent_name, child_name, action_name);

        auto action_find = action_map.find(action_name);
        if(action_find == action_map.end())
        {
            action_2_mat[GlobalHashTable::HashString("StateMachine"_hash, action_name)] = action_count;
            action_map[action_name] = action_count;
            action_count++;
        }

        auto node_parent = node_map.find(parent_name);
        auto node_child = node_map.find(child_name);
        if(node_parent == node_map.end())
        {
            node_map[parent_name] = CreateRef<StateNode>(parent_name);
            node_map[parent_name]->transferFun = transfer;

            node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, parent_name)] = node_count;
            node_count++;
        }
        
        if(node_child == node_map.end())
        {
            node_map[child_name] = CreateRef<StateNode>(child_name);
            node_map[child_name]->transferFun = transfer;
            
            node_2_mat[GlobalHashTable::HashString("StateMachine"_hash, child_name)] = node_count;
            node_count++;
        }

        edge_map[edge_name] = CreateRef<StateEdge>(edge_name);
        edge_map[edge_name]->parent = node_map[parent_name];
        edge_map[edge_name]->child = node_map[child_name];
        edge_map[edge_name]->actionFun = action;
        edge_map[edge_name]->action_name = action_name;

        action_node_pair[GlobalHashTable::HashString("StateMachine"_hash, edge_name)] = {
            GlobalHashTable::HashString("StateMachine"_hash, parent_name),
            GlobalHashTable::HashString("StateMachine"_hash, action_name)
        };

        node_map[parent_name]->AddEgde(edge_map[edge_name]);

        ++it_yaml;
    }

    // Make Transfer Mat
    transfer_edge_mat = Eigen::Matrix<uint64_t, Eigen::Dynamic, Eigen::Dynamic>(node_count, action_count);

    for(int i = 0; i < node_count; ++i)
        for(int j = 0; j < action_count; ++j)
            transfer_edge_mat(i, j) = GlobalHashTable::HashString("StateMachine"_hash, "null");

    for(auto it = action_node_pair.begin(); it != action_node_pair.end(); ++it)
        transfer_edge_mat(node_2_mat[it->second.first], action_2_mat[it->second.second]) = it->first;

    Ref<StateMachine> stateMachine = CreateRef<StateMachine>(name);
    stateMachine->SetInitState(node_map[init_name]);
    return stateMachine;
}

void StateMachineSerializer::Serialize(const String& name, Ref<StateMachine> stateMachine)
{
    String asset_path = g_AssetLoader->GetAssetPath();
    String full_path = asset_path + name;

    UMap<String, Ref<StateNode>> node_map;
    UMap<String, Ref<StateNode>> end_node_map;
    UMap<String, Ref<StateEdge>> edge_map;

    auto init = stateMachine->GetInitNode();
    node_map[init->name] = init;

    while(!node_map.empty())
    {
        auto it = node_map.begin();
        for(auto edge = it->second->edgeList.begin(); edge != it->second->edgeList.end(); ++edge)
        {
            edge_map[edge->second->name] = edge->second;
            auto find_node = end_node_map.find(edge->second->child->name);
            if(find_node == end_node_map.end())
            {
                node_map.insert({edge->second->child->name, edge->second->child});
            }
        }
        node_map.erase(it);
        end_node_map[it->first] = it->second;
    }

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "init_node";
    out << YAML::Value << init->name;

    out << YAML::Key << "edge";
    out << YAML::Value;

    out << YAML::BeginSeq;
    uint32_t edge_count = 0;
    
    for(auto it = edge_map.begin(); it != edge_map.end(); ++it)
    {
        edge_count++;

        out << YAML::BeginMap;
        out << YAML::Key << "edge" << YAML::Value << edge_count;
        out << YAML::Key << "name" << YAML::Value << it->second->name;
        out << YAML::Key << "parent" << YAML::Value << it->second->parent->name;
        out << YAML::Key << "child" << YAML::Value << it->second->child->name;
        out << YAML::Key << "action" << YAML::Value << it->second->action_name;
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    String output = out.c_str();
    //std::cout << output << std::endl;
    RK_CORE_TRACE("Serialize String Size : {}, {}", output.size(), out.size());
    g_AssetLoader->SyncOpenAndWriteStringToTextFile(name, output);
}
