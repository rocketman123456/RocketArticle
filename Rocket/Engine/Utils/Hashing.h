#pragma once
#include <string>
#include <ostream>
#include <functional>

namespace Rocket
{
    struct _H_
    {
        std::string s1, s2;
    };

    inline std::ostream &operator<<(std::ostream& os, const _H_& h)
    {
        os << h.s1 << "," << h.s2;
        return os;
    }
}

namespace std
{
    template <>
	struct hash<Rocket::_H_>
	{
		size_t operator()(const Rocket::_H_& h) const
		{
			return std::hash<std::string>{}(h.s1) ^ (std::hash<std::string>{}(h.s2) << 1);
		}
	};
}