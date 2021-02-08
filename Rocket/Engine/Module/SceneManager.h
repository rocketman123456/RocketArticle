#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Allocator.h"
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

        [[nodiscard]] bool AddScene(Ref<Scene> scene);
        [[nodiscard]] bool RemoveScene(const String& name);
        [[nodiscard]] Ref<Scene>& GetActiveScene() { return m_ActiveScene; }
        [[nodiscard]] bool SetActiveScene(const String& name);
    private:
        UMap<uint64_t, Ref<Scene>> m_SceneList;
        Ref<Scene> m_ActiveScene;
    };

    SceneManager* GetSceneManager();
    extern SceneManager* g_SceneManager;
}