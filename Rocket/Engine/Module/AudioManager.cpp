#include "Module/AudioManager.h"
#include "Module/ProcessManager.h"

using namespace Rocket;

AudioManager* Rocket::GetAudioManager() { return new AudioManager(); }

int AudioManager::Initialize()
{
    const ALCchar* name;
    ALCdevice* device = nullptr;
    ALCcontext* ctx;

    if (!device)
        device = alcOpenDevice(NULL);
    if (!device)
    {
        RK_CORE_ERROR("Could not open a device!");
        return 1;
    }

    // Check for EAX 2.0 support
    auto bEAX = alIsExtensionPresent("EAX2.0");

    ctx = alcCreateContext(device, NULL);
    if (ctx == NULL || alcMakeContextCurrent(ctx) == ALC_FALSE)
    {
        if (ctx != NULL)
            alcDestroyContext(ctx);
        alcCloseDevice(device);
        RK_CORE_ERROR("Could not set a context!");
        return 1;
    }

    name = NULL;
    if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(device) != AL_NO_ERROR)
        name = alcGetString(device, ALC_DEVICE_SPECIFIER);
    RK_CORE_INFO("Opened \"{0}\"", name);

    return 0;
}

void AudioManager::Finalize()
{
    for (auto it = m_AudioStore.begin(); it != m_AudioStore.end(); ++it)
    {
        alDeleteSources(1, &it->second.source);
        alDeleteBuffers(1, &it->second.buffer);
    }

    ALCdevice* device;
    ALCcontext* ctx;

    ctx = alcGetCurrentContext();
    if (ctx == NULL)
        return;

    device = alcGetContextsDevice(ctx);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(device);
}

void AudioManager::Tick(Timestep ts)
{
}

void AudioManager::LoadAudio(const std::string& filename)
{
    std::string name;
    name = filename;
    
    // Check If Resource is Loaded
    auto it = m_AudioStore.find(name);
    if (it != m_AudioStore.end())
    {
        RK_CORE_INFO("Audio Resource {0} is Loaded", name);
        return;
    }
    
    ALuint buffer = Load(filename);
    ALuint source = 0;
    
    RK_CORE_ASSERT(alGetError() == AL_NO_ERROR, "Failed to setup sound source");
    
    m_AudioStore[name] = { buffer, source };
}

ALuint AudioManager::Load(const String& filename)
{
    ALenum err, format;
    ALuint buffer;
    SNDFILE* sndfile;
    SF_INFO sfinfo;
    short* membuf;
    sf_count_t num_frames;
    ALsizei num_bytes;

    // Open the audio file and check that it's usable.
    sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!sndfile)
    {
        RK_CORE_ERROR("Could not open audio in {0}: {1}", filename, sf_strerror(sndfile));
        return 0;
    }
    if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
    {
        RK_CORE_ERROR("Bad sample count in {0}", filename);
        sf_close(sndfile);
        return 0;
    }

    // Get the sound format, and figure out the OpenAL format
    format = AL_NONE;
    if (sfinfo.channels == 1)
        format = AL_FORMAT_MONO16;
    else if (sfinfo.channels == 2)
        format = AL_FORMAT_STEREO16;
    else if (sfinfo.channels == 3)
    {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            format = AL_FORMAT_BFORMAT2D_16;
    }
    else if (sfinfo.channels == 4)
    {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
            format = AL_FORMAT_BFORMAT3D_16;
    }
    if (!format)
    {
        RK_CORE_ERROR("Unsupported channel count: {0}", sfinfo.channels);
        sf_close(sndfile);
        return 0;
    }

    // Decode the whole audio file to a buffer.
    auto size = (size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short);
    membuf = (short*)malloc(size);

    num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
    if (num_frames < 1)
    {
        free(membuf);
        sf_close(sndfile);
        RK_CORE_ERROR("Failed to read samples in {0}", filename);
        return 0;
    }
    num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

    // Buffer the audio data into a new buffer object, then free the data and
    // close the file.
    buffer = 0;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

    free(membuf);
    sf_close(sndfile);

    // Check if an error occured, and clean up if so.
    err = alGetError();
    if (err != AL_NO_ERROR)
    {
        RK_CORE_ERROR("OpenAL Error: {0}", alGetString(err));
        if (buffer && alIsBuffer(buffer))
            alDeleteBuffers(1, &buffer);
        return 0;
    }

    return buffer;
}

void AudioManager::PlayAudio(const String& name)
{
    // Check Resource Load
    auto it = m_AudioStore.find(name);
    if (it != m_AudioStore.end())
    {
        auto buffer = m_AudioStore[name].buffer;
        //g_ProcessManager->AttachProcess(CreateRef<AudioProcess>(buffer));
        //std::thread _thread(&AudioManager::Play, this, std::ref(m_AudioStore[name]));
        //if(_thread.joinable())
        //    _thread.detach();
    }
    else
    {
        RK_CORE_ERROR("Sound {0} Doesn't Exist!", name);
    }
}

void AudioManager::Play(AudioInfo& info)
{
    ALenum state;
    ALuint source = 0;

    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, (ALint)info.buffer);

    alSourcePlay(source);
    do
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (alGetError() == AL_NO_ERROR && state == AL_PLAYING);

    alDeleteSources(1, &source);
}