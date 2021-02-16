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

    ENUM(AssetOpenMode)
    {
        RK_OPEN_TEXT = 0,    /// Open In Text Mode
        RK_OPEN_BINARY = 1,  /// Open In Binary Mode
        RK_WRITE_TEXT,       /// Write In Text Mode
        RK_WRITE_BINARY,     /// Write In Binary Mode
    };

    ENUM(AssetSeekBase)
    {
        RK_SEEK_SET = 0,  /// SEEK_SET
        RK_SEEK_CUR = 1,  /// SEEK_CUR
        RK_SEEK_END = 2   /// SEEK_END
    };

    ENUM(AssetType)
    {
        RK_NORMAL = 0,
        RK_PICTURE,
        RK_AUDIO
    };

    class AssetLoader : implements IRuntimeModule
    {
    public:
        RUNTIME_MODULE_TYPE(AssetLoader);
        AssetLoader() = default;
        virtual ~AssetLoader() = default;

        [[nodiscard]] int Initialize() final;
        void Finalize() final;

        void Tick(Timestep ts) final;

        virtual AssetFilePtr SyncOpenAndReadTexture(const String& filePath, int32_t* width, int32_t* height, int32_t* channels, int32_t desired_channel = 0);
        virtual void SyncCloseTexture(AssetFilePtr data);
        virtual void SyncOpenAndReadAudio(const String& filePath, uint32_t* buffer);
        virtual void SyncCloseAudio(uint32_t* buffer);

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

        virtual String SyncOpenAndReadTextFileToString(const String& fileName);
        virtual bool SyncOpenAndWriteStringToTextFile(const String& fileName, const String& content);

        const String& GetAssetPath() { return m_AssetPath; }

    private:
        String m_AssetPath;
    };

    AssetLoader* GetAssetLoader();
    extern AssetLoader* g_AssetLoader;
}