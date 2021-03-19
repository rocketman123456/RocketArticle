#pragma once
#include "Interface/IRuntimeModule.h"
#include "Common/Buffer.h"

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include <gli/gli.hpp>

namespace Rocket
{
    using AssetFilePtr = void*;
    using Texture2DAsset = gli::texture2d;
    using TextureCubeAsset = gli::texture_cube;
    using Texture2DPtr = Ref<gli::texture2d>;
    using TextureCubePtr = Ref<gli::texture_cube>;

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

        // stb_image
        // Support JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
        // PNG 1 / 2 / 4 / 8 / 16 - bit - per - channel
        // TGA(not sure what subset, if a subset)
        // BMP non - 1bpp, non - RLE
        // PSD(composited view only, no extra channels, 8 / 16 bit - per - channel)
        // GIF(*comp always reports as 4 - channel)
        // HDR(radiance rgbE format)
        // PIC(Softimage PIC)
        // PNM(PPM and PGM binary only)
        virtual AssetFilePtr SyncOpenAndReadTexture(const String& file_path, int32_t* width, int32_t* height, int32_t* channels, int32_t desired_channel = 0);
        virtual void SyncCloseTexture(AssetFilePtr data);
        virtual Vec<AssetFilePtr> SyncOpenAndReadTextureCube(const Vec<String>& file_paths, int32_t* width, int32_t* height, int32_t* channels, int32_t desired_channel = 0);
        virtual void SyncCloseTextureCube(Vec<AssetFilePtr>& datas);

        // gli
        // Only support DDS, KTX or KMG
        virtual Texture2DAsset SyncLoadTexture2D(const String& filename);
        virtual TextureCubeAsset SyncLoadTextureCube(const String& filename);

        // Support http://libsndfile.github.io/libsndfile/formats.html
        virtual void SyncOpenAndReadAudio(const String& file_path, uint32_t* buffer);
        virtual void SyncCloseAudio(uint32_t* buffer);

        // Normal File Read/Write Functions
        virtual Buffer SyncOpenAndReadText(const String& file_path);
        virtual Buffer SyncOpenAndReadBinary(const String& file_path);
        virtual bool SyncOpenAndWriteText(const String& file_path, const Buffer& buf);
        virtual bool SyncOpenAndWriteBinary(const String& file_path, const Buffer& buf);
        virtual String SyncOpenAndReadTextFileToString(const String& file_name);
        virtual bool SyncOpenAndWriteStringToTextFile(const String& file_name, const String& content);

        // Basic File Operation
        virtual size_t SyncRead(const AssetFilePtr& fp, Buffer& buf);
        virtual size_t SyncWrite(const AssetFilePtr& fp, Buffer& buf);
        virtual AssetFilePtr OpenFile(const String& name, AssetOpenMode mode);
        virtual void CloseFile(AssetFilePtr& fp);
        virtual size_t GetSize(const AssetFilePtr& fp);
        virtual int32_t Seek(AssetFilePtr fp, long offset, AssetSeekBase where);

        const String& GetAssetPath() { return asset_path_; }
    private:
        String asset_path_;
    };

    AssetLoader* GetAssetLoader();
    extern AssetLoader* g_AssetLoader;
}