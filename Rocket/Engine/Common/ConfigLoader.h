#pragma once
#include "Core/Core.h"

#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <sstream>

namespace Rocket
{
    class ConfigLoader
    {
    public:
        ConfigLoader(const std::string& path) : m_Path(path) {}
        ~ConfigLoader() = default;

        int Initialize()
        {
            std::string config_file;
#if defined(PLATFORM_LINUX)
            config_file = m_Path + "/Config/setting-linux.yaml";
#elif defined(PLATFORM_WINDOWS)
            config_file = m_Path + "/Config/setting-windows.yaml";
#elif defined(PLATFORM_APPLE)
            config_file = m_Path + "/Config/setting-mac.yaml";
#endif
            m_ConfigMap["Path"] = YAML::LoadFile(config_file);
            config_file = m_Path + "/Config/setting-graphics.yaml";
            m_ConfigMap["Graphics"] = YAML::LoadFile(config_file);
            config_file = m_Path + "/Config/setting-event.yaml";
            m_ConfigMap["Event"] = YAML::LoadFile(config_file);
            return 0;
        }

        std::string GetAssetPath()
        {
            return m_ConfigMap["Path"]["asset_path"].as<std::string>();
        }

        template<typename T>
        T GetConfigInfo(const std::string& category, const std::string& name)
        {
            return m_ConfigMap[category][name].as<T>();
        }

        std::string ToString() const
		{
			std::stringstream ss;
            ss << "Config Path : " << m_Path;
			return ss.str();
		}

    private:
        std::string m_Path;
        std::unordered_map<std::string, YAML::Node> m_ConfigMap;
    };

    inline std::ostream& operator << (std::ostream &os, const ConfigLoader& c)
	{
		os << c.ToString();
		return os;
	}
}