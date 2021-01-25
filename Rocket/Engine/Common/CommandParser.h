#pragma once
#include "Core/Core.h"

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
        }
        ~CommandParser() { m_CommandList.clear(); }

        inline const std::string& GetCommand(int index) 
        {
            RK_CORE_ASSERT(index < m_CommandCount, "Invalid Command Index");
            return m_CommandList[index];
        }

        std::string ToString() const
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
        Vec<std::string> m_CommandList;
    };
}