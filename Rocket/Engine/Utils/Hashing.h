#pragma once
#include "Core/Core.h"

#include <string>
#include <ostream>
#include <functional>
#include <unordered_map>

namespace std
{
    struct _H_ { std::string s1, s2; };
    inline std::ostream &operator<<(std::ostream& os, const _H_& h) { os << h.s1 << "," << h.s2; return os; }

    template <>
	struct hash<_H_> { size_t operator()(const _H_& h) const { return std::hash<std::string>{}(h.s1) ^ (std::hash<std::string>{}(h.s2) << 1); } };
}

namespace Rocket
{
    class HashFunction
    {
    public:
        template<typename T>
        [[nodiscard]] static uint64_t Hash(const T& t) { return std::hash<T>{}(t); }
    };

    class GlobalHashTable 
    {
    public:
        [[nodiscard]] static uint64_t HashString(const uint64_t type_id, const String& str);
        [[nodiscard]] static const String& GetStringFromId(const uint64_t type_id, uint64_t id);
    protected:
        static UMap<uint64_t, UMap<uint64_t, String>> id_string_map;
    };

    namespace details
    {
        template<class>struct hasher;
        template<>
        struct hasher<std::string>
        {
            uint64_t constexpr operator()(char const *input) const { return *input ? static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) : 5381; }
            uint64_t operator()( const std::string& str ) const { return (*this)(str.c_str()); }
        };

        constexpr int32_t i32(const char *s, int32_t v) { return *s ? i32(s + 1, v * 256 + *s) : v; }
        constexpr uint16_t u16(const char *s, uint16_t v) { return *s ? u16(s + 1, v * 256 + *s) : v; }
        constexpr uint32_t u32(const char *s, uint32_t v) { return *s ? u32(s + 1, v * 256 + *s) : v; }
    }
    
    constexpr int32_t operator"" _i32(const char *s, size_t) { return details::i32(s, 0); }
    constexpr uint32_t operator"" _u32(const char *s, size_t) { return details::u32(s, 0); }
    constexpr uint16_t operator"" _u16(const char *s, size_t) { return details::u16(s, 0); }

    template<typename T>
    constexpr uint64_t hash(T&& t) { return details::hasher< typename std::decay<T>::type >()(std::forward<T>(t)); }
    constexpr uint64_t operator "" _hash(const char* s, size_t) { return details::hasher<std::string>()(s); }
}