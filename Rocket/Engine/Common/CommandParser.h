#pragma once
#include "Core/Core.h"
#include <sstream>

namespace Rocket
{
    class CommandParser
    {
    public:
        CommandParser(int argc, char **argv) : command_count_(argc)
        {
            command_list_.clear();
            for(int i = 0; i < argc; ++i)
            {
                command_list_.push_back(argv[i]);
            }
            Parse();
        }
        ~CommandParser() { command_list_.clear(); }

        inline const String& get_command(int index)
        {
            RK_CORE_ASSERT(index < command_count_, "Invalid Command Index");
            return command_list_[index];
        }

        inline const String& get_command(const String& var)
        {
            auto it = command_map.find(var);
            RK_CORE_ASSERT(it != command_map.end(), "Invalid Command Index");
            return it->second;
        }

        // TODO : implements command parser
        void Parse()
        {
        }

        String ToString() const
		{
			std::stringstream ss;
            ss << "Command : ";
            for(auto command : command_list_)
            {
                ss << "[" << command << "]";
            }
			return ss.str();
		}

    private:
        int32_t command_count_;
        Vec<String> command_list_;
        UMap<String, String> command_map;
    };

    inline std::ostream &operator << (std::ostream &os, const CommandParser &c)
	{
		os << c.ToString();
		return os;
	}
}