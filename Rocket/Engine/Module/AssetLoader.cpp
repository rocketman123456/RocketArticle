#include "Module/AssetLoader.h"
#include "Module/Application.h"
#include "Module/MemoryManager.h"

using namespace Rocket;

AssetLoader* Rocket::GetAssetLoader()
{
    return new AssetLoader();
}

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
    {
        RK_CORE_ERROR("Error opening file '{}'", filePath);
    }

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
    {
        RK_CORE_ERROR("Error opening file '{}'", filePath);
    }

    return buff;
}

bool AssetLoader::SyncOpenAndWriteText(const String& filePath, const Buffer& buf)
{
    AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_TEXT);
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
    AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_TEXT);
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
