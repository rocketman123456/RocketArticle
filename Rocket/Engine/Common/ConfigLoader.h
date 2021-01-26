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

        std::string GetAssetPath()
        {
            return m_Config["asset_path"].as<std::string>();
        }

        template<typename T>
        T GetConfig(const std::string& name)
        {
            return m_Config[name].as<T>();
        }

    private:
        std::string m_Path;
        YAML::Node m_Config;
    };
}