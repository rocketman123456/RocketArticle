#pragma once
#include "Core/Core.h"

#include <yaml-cpp/yaml.h>
#include <sstream>

namespace Rocket
{
    class ConfigLoader
    {
    public:
        ConfigLoader(const String& path) : path_(path) {}
        ~ConfigLoader() = default;

        int Initialize()
        {
            String config_file;
#if defined(PLATFORM_LINUX)
            config_file = path_ + "/Config/setting-linux.yaml";
#elif defined(PLATFORM_WINDOWS)
            config_file = path_ + "/Config/setting-windows.yaml";
#elif defined(PLATFORM_APPLE)
            config_file = path_ + "/Config/setting-mac.yaml";
#else
            RK_CORE_ASSERT(false, "Unknown Platform");
#endif
            config_map_["Path"] = YAML::LoadFile(config_file);
            config_file = path_ + "/Config/setting-graphics.yaml";
            config_map_["Graphics"] = YAML::LoadFile(config_file);
            config_file = path_ + "/Config/setting-event.yaml";
            config_map_["Event"] = YAML::LoadFile(config_file);
            config_file = path_ + "/Config/setting-serialport.yaml";
            config_map_["SerialPort"] = YAML::LoadFile(config_file);
            return 0;
        }

        String GetAssetPath()
        {
            return config_map_["Path"]["asset_path"].as<String>();
        }

        template<typename T>
        T GetConfigInfo(const String& category, const String& name)
        {
            return config_map_[category][name].as<T>();
        }

        String ToString() const
		{
			std::stringstream ss;
            ss << "Config Path : " << path_;
			return ss.str();
		}

    private:
        String path_;
        UMap<String, YAML::Node> config_map_;
    };

    inline std::ostream& operator << (std::ostream &os, const ConfigLoader& c)
	{
		os << c.ToString();
		return os;
	}
}