#pragma once

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>

namespace Rocket
{
	template <typename Base, typename T>
	static bool InstanceOf (const T *)
	{
		return std::is_base_of<Base, T>::value;
	}

    template <typename T>
    using Vec = std::vector<T>;

	template <typename T1, typename T2>
	using Map = std::map<T1, T2>;
	template <typename T1, typename T2>
	using UMap = std::unordered_map<T1, T2>;

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T>
	using StoreRef = std::weak_ptr<T>;
} // namespace Rocket