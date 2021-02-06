#pragma once
#include "Core/Core.h"
#include <sstream>

namespace Rocket
{
    class CommandParser
    {
    public:
        CommandParser(int argc, char **argv) : m_CommandCount(argc)
        {
            m_CommandList.clear();
            for(int i = 0; i < argc; ++i)
            {
                m_CommandList.push_back(argv[i]);
            }
            Parse();
        }
        ~CommandParser() { m_CommandList.clear(); }

        inline const String& GetCommand(int index)
        {
            RK_CORE_ASSERT(index < m_CommandCount, "Invalid Command Index");
            return m_CommandList[index];
        }

        inline const String& GetCommand(const String& var)
        {
            auto it = m_CommandMap.find(var);
            RK_CORE_ASSERT(it != m_CommandMap.end(), "Invalid Command Index");
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
            for(auto command : m_CommandList)
            {
                ss << "[" << command << "]";
            }
			return ss.str();
		}

    private:
        int32_t m_CommandCount;
        Vec<String> m_CommandList;
        UMap<String, String> m_CommandMap;
    };

    inline std::ostream &operator << (std::ostream &os, const CommandParser &c)
	{
		os << c.ToString();
		return os;
	}
}