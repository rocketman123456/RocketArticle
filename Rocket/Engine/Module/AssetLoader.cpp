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

    Buffer AssetLoader::SyncOpenAndReadText(const std::string& filePath)
    {
        AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_TEXT);
        Buffer buff;

        if(fp)
        {
            size_t length = GetSize(fp);

            uint8_t* data = new uint8_t[length + 1];
            length = fread(data, 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
            RK_CORE_TRACE("Read file '{}', {} bytes", filePath, length);
#endif
            data[length] = '\0';
            buff.SetData(data, length);

            CloseFile(fp);
        } else {
            RK_CORE_ERROR("Error opening file '{}'", filePath);
        }

        return buff;
    }

    Buffer AssetLoader::SyncOpenAndReadBinary(const std::string& filePath)
    {
        AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_BINARY);
        Buffer buff;

        if(fp)
        {
            size_t length = GetSize(fp);

            uint8_t* data = new uint8_t[length + 1];
            length = fread(data, 1, length, static_cast<FILE*>(fp));
#ifdef RK_DEBUG
            RK_CORE_TRACE("Read file '{}', {} bytes", filePath, length);
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

    bool AssetLoader::SyncOpenAndWriteText(const std::string& filePath, const Buffer& buf)
    {
        AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_TEXT);
        if(!fp)
        {
            RK_CORE_ERROR("Write Text to File Open File Error");
            return false;
        }
        auto sz = fputs((char*)(buf.GetData()), static_cast<FILE*>(fp));
        CloseFile(fp);
        return true;
    }

    bool AssetLoader::SyncOpenAndWriteBinary(const std::string& filePath, const Buffer& buf)
    {
        AssetFilePtr fp = OpenFile(filePath, AssetOpenMode::RK_OPEN_TEXT);
        if(!fp)
        {
            RK_CORE_ERROR("Write Text to File Open File Error");
            return false;
        }
        size_t sz = fwrite(buf.GetData(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));
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

        size_t sz = fread(buf.GetData(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));
        return sz;
    }

    size_t AssetLoader::SyncWrite(const AssetFilePtr& fp, Buffer& buf)
    {
        if(!fp)
        {
            RK_CORE_ERROR("null file discriptor");
            return 0;
        }

        size_t sz = fwrite(buf.GetData(), buf.GetDataSize(), 1, static_cast<FILE*>(fp));
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