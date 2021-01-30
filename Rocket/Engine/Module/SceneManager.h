#pragma once
#include "Interface/IRuntimeModule.h"
#include "Scene/Scene.h"

#include <unordered_map>

namespace Rocket
{
    class SceneManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(SceneManager);
        SceneManager() = default;
        virtual ~SceneManager() = default;

        int Initialize() final;
        void Finalize() final;
        void Tick(Timestep ts) final;

        [[nodiscard]] bool AddScene(const std::string& name, Ref<Scene> scene);
        [[nodiscard]] bool RemoveScene(const std::string& name);
        [[nodiscard]] Ref<Scene> GetActiveScene() { return m_ActiveScene; }
        [[nodiscard]] bool SetActiveScene(const std::string& name);
    private:
        std::unordered_map<std::string, Ref<Scene>> m_SceneList;
        Ref<Scene> m_ActiveScene;
    };

    SceneManager* GetSceneManager();
    extern SceneManager* g_SceneManager;
}