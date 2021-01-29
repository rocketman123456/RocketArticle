#include "Module/AssetLoader.h"
#include "Module/Application.h"

namespace Rocket
{
    AssetLoader* GetAssetLoader()
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

    AssetFilePtr AssetLoader::OpenFile(const std::string& name, AssetOpenMode mode)
    {
        FILE* fp = nullptr;
        std::string fullPath = m_AssetPath + name;

        switch(mode)
        {
        case RK_OPEN_TEXT:
            fp = fopen(fullPath.c_str(), "r");
            break;
        case RK_OPEN_BINARY:
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

    Buffer AssetLoader::SyncOpenAndReadText(const std::string& filePath)
    {
        AssetFilePtr fp = OpenFile(filePath, RK_OPEN_TEXT);
        Buffer buff;

        if(fp)
        {
            size_t length = GetSize(fp);

            uint8_t* data = new uint8_t[length + 1];
            length = fread(data, 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
            RK_CORE_INFO("Read file '{}', {} bytes", filePath, length);
#endif
            buff.SetData(data, length);

            CloseFile(fp);
        } else {
            RK_CORE_ERROR("Error opening file '{}'", filePath);
        }

        return buff;
    }

    Buffer AssetLoader::SyncOpenAndReadBinary(const std::string& filePath)
    {
        AssetFilePtr fp = OpenFile(filePath, RK_OPEN_BINARY);
        Buffer buff;

        if(fp)
        {
            size_t length = GetSize(fp);

            uint8_t* data = new uint8_t[length + 1];
            length = fread(data, 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
            RK_CORE_INFO("Read file '{}', {} bytes", filePath, length);
#endif
            data[length] = '\0';
            buff.SetData(data, length + 1);

            CloseFile(fp);
        }
        else
        {
            RK_CORE_ERROR("Error opening file '{}'", filePath);
        }

        return buff;
    }

    size_t AssetLoader::SyncRead(const AssetFilePtr& fp, Buffer& buf)
    {
        size_t sz;

        if(!fp)
        {
            RK_CORE_ERROR("null file discriptor");
            return 0;
        }

        sz = fread(buf.GetData(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));

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
}