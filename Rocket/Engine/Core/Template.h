#pragma once

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <queue>
#include <array>

namespace Rocket
{
	template <typename Base, typename T>
	static bool InstanceOf (const T *)
	{
		return std::is_base_of<Base, T>::value;
	}

	template <typename T>
    //using Scope = std::unique_ptr<T, std::function<void(T*)>>;
    using Scope = std::unique_ptr<T>;

    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T>
    using StoreRef = std::weak_ptr<T>;

    template <typename T, typename... Arguments>
    constexpr Ref<T> CreateRef(Arguments ... args)
    {
        Ref<T> ptr = Ref<T>(
            new T(args...) //new (RK_ALLOCATE_CLASS(T)) T(args...)//, [](T* ptr) { RK_DELETE(T, ptr); }
        );
        return std::move(ptr);
    }

    template <typename T, typename... Arguments>
	constexpr Scope<T> CreateScope(Arguments ... args)
	{
        Scope<T> ptr = Scope<T>(
            new T(args...) //new (RK_ALLOCATE_CLASS(T)) T(args...)//, [](T* ptr) { RK_DELETE(T, ptr); }
        );
        return std::move(ptr);
	}

    template <typename T>
    using Vec = std::vector<T>;

	template <typename T1, typename T2>
	using Map = std::map<T1, T2>;
	template <typename T1, typename T2>
	using UMap = std::unordered_map<T1, T2>;

	template <typename T1, auto T2>
	using Array = std::array<T1, T2>;

	template <typename T>
	using List = std::list<T>;

	template <typename T>
	using Queue = std::queue<T>;

	template <typename T>
	using Set = std::set<T>;
	template <typename T>
	using USet = std::unordered_set<T>;

	using String = std::string;
} // namespace Rocket