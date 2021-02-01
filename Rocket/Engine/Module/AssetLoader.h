#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Buffer.h"

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

namespace Rocket
{
    using AssetFilePtr = void*;

    class AssetLoader : implements IRuntimeModule
    {
    public:
        enum class AssetOpenMode
        {
            RK_OPEN_TEXT = 0,    /// Open In Text Mode
            RK_OPEN_BINARY = 1,  /// Open In Binary Mode
        };

        enum class AssetSeekBase
        {
            RK_SEEK_SET = 0,  /// SEEK_SET
            RK_SEEK_CUR = 1,  /// SEEK_CUR
            RK_SEEK_END = 2   /// SEEK_END
        };
        
        RUNTIME_MODULE_TYPE(AssetLoader);
        AssetLoader() = default;
        virtual ~AssetLoader() = default;

        [[nodiscard]] int Initialize() final;
        void Finalize() final;

        void Tick(Timestep ts) final;

        virtual AssetFilePtr OpenFile(const std::string& name, AssetOpenMode mode);
        virtual Buffer SyncOpenAndReadText(const std::string& filePath);
        virtual Buffer SyncOpenAndReadBinary(const std::string& filePath);
        virtual bool SyncOpenAndWriteText(const std::string& filePath, const Buffer& buf);
        virtual bool SyncOpenAndWriteBinary(const std::string& filePath, const Buffer& buf);
        virtual size_t SyncRead(const AssetFilePtr& fp, Buffer& buf);
        virtual size_t SyncWrite(const AssetFilePtr& fp, Buffer& buf);
        virtual void CloseFile(AssetFilePtr& fp);
        virtual size_t GetSize(const AssetFilePtr& fp);
        virtual int32_t Seek(AssetFilePtr fp, long offset, AssetSeekBase where);

        inline std::string SyncOpenAndReadTextFileToString(const std::string& fileName)
        {
            std::string result;
            Buffer buffer = SyncOpenAndReadText(fileName);
            if (buffer.GetDataSize())
            {
                char* content = reinterpret_cast<char*>(buffer.GetData());
                if (content)
                    result = std::string(content);
            }
            return result;
        }

        inline bool SyncOpenAndWriteStringToTextFile(const std::string& fileName, const std::string& content)
        {
            Buffer buf;
            size_t sz = content.size();
            uint8_t* data = new uint8_t[sz + 1];
            memcpy(data, content.data(), sz);
            data[sz] = '\0';
            bool result = SyncOpenAndWriteText(fileName, buf);
            return result;
        }

    private:
        std::string m_AssetPath;
    };

    AssetLoader* GetAssetLoader();
    extern AssetLoader* g_AssetLoader;
}