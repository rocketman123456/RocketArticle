#include "Module/SceneManager.h"
#include "Utils/Hashing.h"

using namespace Rocket;

SceneManager* Rocket::GetSceneManager() { return new SceneManager(); }

int SceneManager::Initialize()
{
    return 0;
}

void SceneManager::SceneManager::Finalize()
{
    scene_list_.clear();
}

void SceneManager::Tick(Timestep ts)
{
    if(active_scene_)
        active_scene_->OnUpdateRuntime(ts);
}

bool SceneManager::SetActiveScene(const String& name)
{
    auto id = GlobalHashTable::HashString("Scene"_hash, name);
    auto findIt = scene_list_.find(id);
    if (findIt == scene_list_.end())
    {
        RK_CORE_ERROR("Find Active Scene Error : {}", name);
        active_scene_ = nullptr;
        return false;
    }
    active_scene_ = findIt->second;
    return true;
}

bool SceneManager::AddScene(Ref<Scene> scene)
{
    auto id = GlobalHashTable::HashString("Scene"_hash, scene->GetName());
    auto findIt = scene_list_.find(id);
    if (findIt != scene_list_.end())
    {
        RK_CORE_WARN("Add Scene Twice With Same Name : {}", scene->GetName());
        return false;
    }
    scene_list_[id] = scene;
    return true;
}

bool SceneManager::RemoveScene(const String& name)
{
    auto id = GlobalHashTable::HashString("Scene"_hash, name);
    auto findIt = scene_list_.find(id);
    if (findIt != scene_list_.end())
    {
        scene_list_.erase(findIt);
        return true;
    }
    RK_CORE_WARN("Cannot Find Scene to Remove : {}", name);
    return false;
}
