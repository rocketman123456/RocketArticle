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
        [[nodiscard]] Ref<Scene>& GetActiveScene() { return active_scene_; }
        [[nodiscard]] bool SetActiveScene(const String& name);
    private:
        UMap<uint64_t, Ref<Scene>> scene_list_;
        Ref<Scene> active_scene_;
    };

    SceneManager* GetSceneManager();
    extern SceneManager* g_SceneManager;
}