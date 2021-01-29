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
        enum AssetOpenMode
        {
            RK_OPEN_TEXT = 0,    /// Open In Text Mode
            RK_OPEN_BINARY = 1,  /// Open In Binary Mode
        };

        enum AssetSeekBase
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
        virtual size_t SyncRead(const AssetFilePtr& fp, Buffer& buf);
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

    private:
        std::string m_AssetPath;
    };

    AssetLoader* GetAssetLoader();
    extern AssetLoader* g_AssetLoader;
}