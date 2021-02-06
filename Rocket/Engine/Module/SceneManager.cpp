#include "Module/SceneManager.h"
#include "Utils/Hashing.h"

using namespace Rocket;

SceneManager* Rocket::GetSceneManager()
{
    return new SceneManager();
}

int SceneManager::Initialize()
{
    return 0;
}

void SceneManager::SceneManager::Finalize()
{
    m_SceneList.clear();
}

void SceneManager::Tick(Timestep ts)
{
    m_ActiveScene->OnUpdateRuntime(ts);
}

bool SceneManager::SetActiveScene(const String& name)
{
    auto id = SceneHashTable::HashString(name);
    auto findIt = m_SceneList.find(id);
    if (findIt == m_SceneList.end())
    {
        RK_CORE_ERROR("Find Active Scene Error : {}", name);
        m_ActiveScene = nullptr;
        return false;
    }
    m_ActiveScene = findIt->second;
    return true;
}

bool SceneManager::AddScene(const String& name, Ref<Scene> scene)
{
    auto id = SceneHashTable::HashString(name);
    auto findIt = m_SceneList.find(id);
    if (findIt != m_SceneList.end())
    {
        RK_CORE_WARN("Add Scene Twice With Same Name : {}", name);
        return false;
    }
    m_SceneList[id] = scene;
    return true;
}

bool SceneManager::RemoveScene(const String& name)
{
    auto id = SceneHashTable::HashString(name);
    auto findIt = m_SceneList.find(id);
    if (findIt != m_SceneList.end())
    {
        m_SceneList.erase(findIt);
        return true;
    }
    RK_CORE_WARN("Cannot Find Scene to Remove : {}", name);
    return false;
}
