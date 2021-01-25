#pragma once
#include "Core/Core.h"

#include <yaml-cpp/yaml.h>

namespace Rocket
{
    class ConfigLoader
    {
    public:
        ConfigLoader(const std::string& path) : m_Path(path) {}
        ~ConfigLoader() = default;

        int Initialize()
        {
            auto config_file = m_Path + "/Config/setting-linux.yaml";
            m_Config = YAML::LoadFile(config_file);
            return 0;
        }

        const std::string& GetAssetPath()
        {
            m_AssetPath = m_Config["asset_path"].as<std::string>();
            return m_AssetPath;
        }

    private:
        std::string m_Path;
        std::string m_AssetPath;
        YAML::Node m_Config;
    };
}