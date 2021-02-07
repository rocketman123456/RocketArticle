#pragma once
#include "Interface/IRuntimeModule.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>

namespace Rocket
{
    struct AudioInfo
    {
        ALuint buffer;
        ALuint source;
    };

    // TODO : Implement AudioManager
    class AudioManager : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(AudioManager);
    public:
        AudioManager() {}
        virtual ~AudioManager() {}

        int Initialize() final;
        void Finalize() final;

        void Tick(Timestep ts) final;

        void LoadAudio(const String& filename);
        void PlayAudio(const String& name);

    private:
        ALuint Load(const String& filename);
        void Play(AudioInfo& info);

    private:
        UMap<String, AudioInfo> m_AudioStore;
    };

    extern AudioManager* g_AudioManager;
    AudioManager* GetAudioManager();
}