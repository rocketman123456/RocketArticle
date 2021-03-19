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
    asset_path_ = config->GetAssetPath();
    return 0;
}

void AssetLoader::Finalize()
{
}

void AssetLoader::Tick(Timestep ts)
{
}

AssetFilePtr AssetLoader::SyncOpenAndReadTexture(const String& file_path, int32_t* width, int32_t* height, int32_t* channels, int32_t desired_channel)
{
    Buffer buffer = SyncOpenAndReadBinary(file_path);

    stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
    data = stbi_load_from_memory(buffer.GetData().get(), buffer.GetDataSize(), width, height, channels, desired_channel);
	RK_CORE_TRACE("Texture Info : width {}, height {}, channels {}", *width, *height, *channels);
    RK_CORE_ASSERT(data, "Failed to load image!");
    return data;
}

void AssetLoader::SyncCloseTexture(AssetFilePtr data)
{
    stbi_image_free(data);
}

Vec<AssetFilePtr> AssetLoader::SyncOpenAndReadTextureCube(const Vec<String>& file_paths, int32_t* width, int32_t* height, int32_t* channels, int32_t desired_channel)
{
    Vec<AssetFilePtr> result;
    for (auto file_name : file_paths)
    {
        Buffer buffer = SyncOpenAndReadBinary(file_name);

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        data = stbi_load_from_memory(buffer.GetData().get(), buffer.GetDataSize(), width, height, channels, desired_channel);
        RK_CORE_TRACE("Texture Info : width {}, height {}, channels {}", *width, *height, *channels);
        RK_CORE_ASSERT(data, "Failed to load image!");
        result.push_back(data);
    }
    return result;
}

void AssetLoader::SyncCloseTextureCube(Vec<AssetFilePtr>& datas)
{
    for (auto img : datas)
    {
        stbi_image_free(img);
    }
}

Texture2DAsset AssetLoader::SyncLoadTexture2D(const String& filename)
{
    String fullPath = asset_path_ + filename;
    auto data = gli::load(fullPath.c_str());
    RK_CORE_TRACE("Open Texture 2D {}, {}, {}", fullPath, data.size(), data.levels());
    Texture2DAsset pic(data);
    RK_CORE_TRACE("Create Texture 2D {}", fullPath);
    return pic;
}

TextureCubeAsset AssetLoader::SyncLoadTextureCube(const String& filename)
{
    String fullPath = asset_path_ + filename;
    auto data = gli::load(fullPath.c_str());
    RK_CORE_TRACE("Open Texture Cube {}", fullPath);
    TextureCubeAsset pic(data);
    RK_CORE_TRACE("Create Texture Cube {}", fullPath);
    return pic;
}

void AssetLoader::SyncOpenAndReadAudio(const String& filename, uint32_t* buffer_in)
{
    String fullPath = asset_path_ + filename;
    RK_CORE_TRACE("Open Audio {}", fullPath);
    
    ALuint buffer = 0;
    ALenum err, format;
    SNDFILE* sndfile;
    SF_INFO sfinfo;
    short* membuf;
    sf_count_t num_frames;
    ALsizei num_bytes;

    // Open the audio file and check that it's usable.
    sndfile = sf_open(fullPath.c_str(), SFM_READ, &sfinfo);
    if (!sndfile)
    {
        RK_CORE_ERROR("Could not open audio in {0}: {1}", fullPath, sf_strerror(sndfile));
        return;
    }
    if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
    {
        RK_CORE_ERROR("Bad sample count in {0}", fullPath);
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
        RK_CORE_ERROR("Failed to read samples in {0}", fullPath);
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

String AssetLoader::SyncOpenAndReadTextFileToString(const String& file_name)
{
    String result;
    Buffer buffer = SyncOpenAndReadText(file_name);
    if (buffer.GetDataSize())
    {
        char* content = reinterpret_cast<char*>(buffer.GetData().get());
        if (content)
            result = String(content);
    }
    return result;
}

bool AssetLoader::SyncOpenAndWriteStringToTextFile(const String& file_name, const String& content)
{
    Buffer buf;
    size_t sz = content.size();
    Ref<uint8_t> data = Ref<uint8_t>(new uint8_t[sz + 1], [](uint8_t* v){ delete[]v; });
    memcpy(data.get(), content.data(), sz);
    data.get()[sz] = '\0';
    buf.SetData(data, sz + 1);
    bool result = SyncOpenAndWriteText(file_name, buf);
    return result;
}

AssetFilePtr AssetLoader::OpenFile(const String& name, AssetOpenMode mode)
{
    FILE* fp = nullptr;
    String fullPath = asset_path_ + name;
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

Buffer AssetLoader::SyncOpenAndReadText(const String& file_path)
{
    AssetFilePtr fp = OpenFile(file_path, AssetOpenMode::RK_OPEN_TEXT);
    Buffer buff;

    if(fp)
    {
        size_t length = GetSize(fp);

        Ref<uint8_t> data = Ref<uint8_t>(new uint8_t[length + 1], [](uint8_t* v){ delete[]v; });
        length = fread(data.get(), 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
        RK_CORE_TRACE("Read file '{}', {} bytes", file_path, length);
#endif
        data.get()[length] = '\0';
        buff.SetData(data, length);

        CloseFile(fp);
    }
    else
        RK_CORE_ERROR("Error opening file '{}'", file_path);

    return buff;
}

Buffer AssetLoader::SyncOpenAndReadBinary(const String& file_path)
{
    AssetFilePtr fp = OpenFile(file_path, AssetOpenMode::RK_OPEN_BINARY);
    Buffer buff;

    if(fp)
    {
        size_t length = GetSize(fp);
        Ref<uint8_t> data = Ref<uint8_t>(new uint8_t[length], [](uint8_t* v){ delete[]v; });
        length = fread(data.get(), 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
        RK_CORE_TRACE("Read file '{}', {} bytes", file_path, length);
#endif
        buff.SetData(data, length);

        CloseFile(fp);
    }
    else
        RK_CORE_ERROR("Error opening file '{}'", file_path);

    return buff;
}

bool AssetLoader::SyncOpenAndWriteText(const String& file_path, const Buffer& buf)
{
    AssetFilePtr fp = OpenFile(file_path, AssetOpenMode::RK_WRITE_TEXT);
    if(!fp)
    {
        RK_CORE_ERROR("Write Text to File Open File Error");
        return false;
    }
    auto sz = fputs((char*)(buf.GetData().get()), static_cast<FILE*>(fp));
    CloseFile(fp);
    return true;
}

bool AssetLoader::SyncOpenAndWriteBinary(const String& file_path, const Buffer& buf)
{
    AssetFilePtr fp = OpenFile(file_path, AssetOpenMode::RK_WRITE_BINARY);
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
