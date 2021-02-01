#pragma once
#include "Core/Core.h"

#include <string>
#include <ostream>
#include <functional>
#include <unordered_map>

namespace std
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

    template <>
	struct hash<_H_>
	{
		size_t operator()(const _H_& h) const
		{
			return std::hash<std::string>{}(h.s1) ^ (std::hash<std::string>{}(h.s2) << 1);
		}
	};
}

namespace Rocket
{
    class HashFunction
    {
    public:
        template<typename T>
        [[nodiscard]] static uint64_t Hash(const T& t)
        {
            return std::hash<T>{}(t);
        }
    };

    class EventHashTable
    {
    public:
        [[nodiscard]] static uint64_t HashString(const std::string& str);
        [[nodiscard]] static const std::string& GetStringFromId(uint64_t id);
    protected:
        static UMap<uint64_t, std::string> IdStringMap;
    };

    class AssetHashTable
    {
    public:
        [[nodiscard]] static uint64_t HashString(const std::string& str);
        [[nodiscard]] static const std::string& GetStringFromId(uint64_t id);
    protected:
        static UMap<uint64_t, std::string> IdStringMap;
    };
}