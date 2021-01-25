#pragma once
#include <memory>
#include <vector>
#include <map>

namespace Rocket
{
	template <typename Base, typename T>
	inline bool InstanceOf (const T *)
	{
		return std::is_base_of<Base, T>::value;
	}

    template <typename T>
    using Vec = std::vector<T>;

	template <typename T>
	using Scope = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args &&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args &&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using StoreRef = std::weak_ptr<T>;

    template <class BaseType, class SubType>
    BaseType* GenericObjectCreationFunction(void) { return new SubType; }

    template <class BaseClass, class IdType>
    class GenericObjectFactory
    {
        typedef BaseClass* (*ObjectCreationFunction)(void);
        std::map<IdType, ObjectCreationFunction> m_creationFunctions;

    public:
        template <class SubClass>
        bool Register(IdType id)
        {
            auto findIt = m_creationFunctions.find(id);
            if (findIt == m_creationFunctions.end())
            {
                m_creationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;  // insert() is giving me compiler errors
                return true;
            }

            return false;
        }

        Ref<BaseClass> Create(IdType id)
        {
            auto findIt = m_creationFunctions.find(id);
            if (findIt != m_creationFunctions.end())
            {
                ObjectCreationFunction pFunc = findIt->second;
                return pFunc();
            }

            return nullptr;
        }
    };
} // namespace Rocket