#include "Module/AssetLoader.h"
#include "Module/Application.h"
#include "Module/MemoryManager.h"

#include <stb_image.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>

using namespace Rocket;

AssetLoader* Rocket::GetAssetLoader() { return new AssetLoader(); }

int AssetLoader::Initialize()
{
    auto config = g_Application->GetConfig();
    m_AssetPath = config->GetAssetPath();
    return 0;
}

void AssetLoader::Finalize()
{
}

void AssetLoader::Tick(Timestep ts)
{
}

AssetFilePtr AssetLoader::SyncOpenAndReadTexture(const String& filePath, int32_t* width, int32_t* height, int32_t* channels)
{
    stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		data = stbi_load(filePath.c_str(), width, height, channels, 0);
	}
    RK_CORE_ASSERT(data, "Failed to load image!");
    return data;
}

void AssetLoader::SyncCloseTexture(AssetFilePtr data)
{
    stbi_image_free(data);
}

void AssetLoader::SyncOpenAndReadAudio(const String& filename, uint32_t* buffer_in)
{
    ALuint buffer = 0;
    ALenum err, format;
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
        return;
    }
    if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
    {
        RK_CORE_ERROR("Bad sample count in {0}", filename);
        sf_close(sndfile);
        return;
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
        return;
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
        return;
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
        return;
    }
    
    RK_CORE_ASSERT(alGetError() == AL_NO_ERROR, "Failed to setup sound source");

    *buffer_in = buffer;
    return;
}

void AssetLoader::SyncCloseAudio(uint32_t* buffer)
{
    alDeleteBuffers(1, buffer);
}

String AssetLoader::SyncOpenAndReadTextFileToString(const String& fileName)
{
    String result;
    Buffer buffer = SyncOpenAndReadText(fileName);
    if (buffer.GetDataSize())
    {
        char* content = reinterpret_cast<char*>(buffer.GetData().get());
        if (content)
            result = String(content);
    }
    return result;
}

bool AssetLoader::SyncOpenAndWriteStringToTextFile(const String& fileName, const String& content)
{
    Buffer buf;
    size_t sz = content.size();
    Ref<uint8_t> data = Ref<uint8_t>(new uint8_t[sz + 1], [](uint8_t* v){ delete[]v; });
    memcpy(data.get(), content.data(), sz);
    data.get()[sz] = '\0';
    bool result = SyncOpenAndWriteText(fileName, buf);
    return result;
}

AssetFilePtr AssetLoader::OpenFile(const String& name, AssetOpenMode mode)
{
    FILE* fp = nullptr;
    String fullPath = m_AssetPath + name;
    RK_CORE_TRACE("Open File Full Path : {}", fullPath);

    switch(mode)
    {
    case AssetOpenMode::RK_OPEN_TEXT:
        fp = fopen(fullPath.c_str(), "r");
        break;
    case AssetOpenMode::RK_OPEN_BINARY:
        fp = fopen(fullPath.c_str(), "rb");
        break;
    case AssetOpenMode::RK_WRITE_TEXT:
        fp = fopen(fullPath.c_str(), "w");
        break;
    case AssetOpenMode::RK_WRITE_BINARY:
        fp = fopen(fullPath.c_str(), "wb");
        break;
    }

    if(fp)
    {
        return (AssetFilePtr)fp;
    }
    else
    {
        RK_CORE_ERROR("Load File [{}] Error", name);
        return nullptr;
    }
}

Buffer AssetLoader::SyncOpenAndReadText(const String& filePath)
{
    AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_TEXT);
    Buffer buff;

    if(fp)
    {
        size_t length = GetSize(fp);

        Ref<uint8_t> data = Ref<uint8_t>(new uint8_t[length + 1], [](uint8_t* v){ delete[]v; });
        length = fread(data.get(), 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
        RK_CORE_TRACE("Read file '{}', {} bytes", filePath, length);
#endif
        data.get()[length] = '\0';
        buff.SetData(data, length);

        CloseFile(fp);
    }
    else
        RK_CORE_ERROR("Error opening file '{}'", filePath);

    return buff;
}

Buffer AssetLoader::SyncOpenAndReadBinary(const String& filePath)
{
    AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_BINARY);
    Buffer buff;

    if(fp)
    {
        size_t length = GetSize(fp);
        Ref<uint8_t> data = Ref<uint8_t>(new uint8_t[length], [](uint8_t* v){ delete[]v; });
        length = fread(data.get(), 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
        RK_CORE_TRACE("Read file '{}', {} bytes", filePath, length);
#endif
        buff.SetData(data, length);

        CloseFile(fp);
    }
    else
        RK_CORE_ERROR("Error opening file '{}'", filePath);

    return buff;
}

bool AssetLoader::SyncOpenAndWriteText(const String& filePath, const Buffer& buf)
{
    AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_WRITE_TEXT);
    if(!fp)
    {
        RK_CORE_ERROR("Write Text to File Open File Error");
        return false;
    }
    auto sz = fputs((char*)(buf.GetData().get()), static_cast<FILE*>(fp));
    CloseFile(fp);
    return true;
}

bool AssetLoader::SyncOpenAndWriteBinary(const String& filePath, const Buffer& buf)
{
    AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_WRITE_BINARY);
    if(!fp)
    {
        RK_CORE_ERROR("Write Text to File Open File Error");
        return false;
    }
    size_t sz = fwrite(buf.GetData().get(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));
    CloseFile(fp);
    return true;
}

size_t AssetLoader::SyncRead(const AssetFilePtr& fp, Buffer& buf)
{
    if(!fp)
    {
        RK_CORE_ERROR("null file discriptor");
        return 0;
    }

    size_t sz = fread(buf.GetData().get(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));
    return sz;
}

size_t AssetLoader::SyncWrite(const AssetFilePtr& fp, Buffer& buf)
{
    if(!fp)
    {
        RK_CORE_ERROR("null file discriptor");
        return 0;
    }

    size_t sz = fwrite(buf.GetData().get(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));
    return sz;
}

void AssetLoader::CloseFile(AssetFilePtr& fp)
{
    fclose((FILE*)fp);
    fp = nullptr;
}

size_t AssetLoader::GetSize(const AssetFilePtr& fp)
{
    FILE* _fp = static_cast<FILE*>(fp);

    long pos = ftell(_fp);
    fseek(_fp, 0, SEEK_END);
    size_t length = ftell(_fp);
    fseek(_fp, pos, SEEK_SET);

    return length;
}

int32_t AssetLoader::Seek(AssetFilePtr fp, long offset, AssetSeekBase where)
{
    return fseek(static_cast<FILE*>(fp), offset, static_cast<int>(where));
}
