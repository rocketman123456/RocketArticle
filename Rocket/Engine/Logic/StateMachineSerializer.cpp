#include "Logic/StateMachineSerializer.h"
#include "Module/AssetLoader.h"

using namespace Rocket;

Ref<StateMachine> StateMachineSerializer::Serialize(const String& name, TransferFunction transfer, ActionFunction action)
{
    String asset_path = g_AssetLoader->GetAssetPath();
    String full_path = asset_path + name;

    YAML::Node root = YAML::LoadFile(full_path);
    UMap<String, Ref<StateNode>> node_map;

    String init_name = root["init_node"].as<String>();

    auto it = root["edge"].begin();
    while(it != root["edge"].end())
    {
        String edge_name = (*it)["name"].as<String>();
        String parent_name = (*it)["parent"].as<String>();
        String child_name = (*it)["child"].as<String>();

        RK_CORE_TRACE("Edge : {}, {}, {}", edge_name, parent_name, child_name);

        auto node_parent = node_map.find(parent_name);
        auto node_child = node_map.find(child_name);
        if(node_parent == node_map.end())
        {
            node_map[parent_name] = CreateRef<StateNode>(parent_name);
            node_map[parent_name]->transferFun = transfer;
        }
        
        if(node_child == node_map.end())
        {
            node_map[child_name] = CreateRef<StateNode>(child_name);
            node_map[parent_name]->transferFun = transfer;
        }

        Ref<StateEdge> edge = CreateRef<StateEdge>(edge_name);
        edge->parent = node_map[parent_name];
        edge->child = node_map[child_name];
        edge->actionFun = action;
        node_map[parent_name]->AddEgde(edge);

        ++it;
    }

    Ref<StateMachine> stateMachine = CreateRef<StateMachine>("Robot Control");
    stateMachine->SetInitState(node_map[init_name]);
    return stateMachine;
}

void StateMachineSerializer::Deserialize(const String& name, Ref<StateMachine> stateMachine)
{
    String asset_path = g_AssetLoader->GetAssetPath();
    String full_path = asset_path + name;
}

