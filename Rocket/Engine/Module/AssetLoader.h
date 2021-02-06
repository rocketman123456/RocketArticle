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
            RK_WRITE_TEXT,       /// Write In Text Mode
            RK_WRITE_BINARY,     /// Write In Binary Mode
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

        virtual AssetFilePtr OpenFile(const String& name, AssetOpenMode mode);
        virtual Buffer SyncOpenAndReadText(const String& filePath);
        virtual Buffer SyncOpenAndReadBinary(const String& filePath);
        virtual bool SyncOpenAndWriteText(const String& filePath, const Buffer& buf);
        virtual bool SyncOpenAndWriteBinary(const String& filePath, const Buffer& buf);
        virtual size_t SyncRead(const AssetFilePtr& fp, Buffer& buf);
        virtual size_t SyncWrite(const AssetFilePtr& fp, Buffer& buf);
        virtual void CloseFile(AssetFilePtr& fp);
        virtual size_t GetSize(const AssetFilePtr& fp);
        virtual int32_t Seek(AssetFilePtr fp, long offset, AssetSeekBase where);

        String SyncOpenAndReadTextFileToString(const String& fileName);
        bool SyncOpenAndWriteStringToTextFile(const String& fileName, const String& content);

    private:
        String m_AssetPath;
    };

    AssetLoader* GetAssetLoader();
    extern AssetLoader* g_AssetLoader;
}