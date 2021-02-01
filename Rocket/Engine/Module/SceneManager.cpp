#include "Module/SceneManager.h"

namespace Rocket
{
    SceneManager* GetSceneManager()
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

    bool SceneManager::SetActiveScene(const std::string& name)
    {
        auto findIt = m_SceneList.find(name);
        if (findIt == m_SceneList.end())
        {
            RK_CORE_ERROR("Find Active Scene Error : {}", name);
            m_ActiveScene = nullptr;
            return false;
        }
        m_ActiveScene = findIt->second;
        return true;
    }

    bool SceneManager::AddScene(const std::string& name, Ref<Scene> scene)
    {
        auto findIt = m_SceneList.find(name);
        if (findIt != m_SceneList.end())
        {
            RK_CORE_WARN("Add Scene Twice With Same Name : {}", name);
            return false;
        }
        m_SceneList[name] = scene;
        return true;
    }

    bool SceneManager::RemoveScene(const std::string& name)
    {
        auto findIt = m_SceneList.find(name);
        if (findIt != m_SceneList.end())
        {
            m_SceneList.erase(findIt);
            return true;
        }
        RK_CORE_WARN("Cannot Find Scene to Remove : {}", name);
        return false;
    }
}