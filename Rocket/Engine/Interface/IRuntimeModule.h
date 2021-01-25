#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"

namespace Rocket
{
    Interface IRuntimeModule
    {
    public:
        IRuntimeModule() {}
        virtual ~IRuntimeModule() = default;

        [[nodiscard]] virtual int Initialize() = 0;
        virtual void Finalize() = 0;

        virtual void Tick(Timestep ts) = 0;

        // For Debug
        virtual const char* GetName() const = 0;
        virtual std::string ToString() const { return GetName(); }
    };

#define RUNTIME_MODULE_TYPE(type) \
    virtual const char* GetName() const override { return #type; }

    inline std::ostream &operator << (std::ostream& os, const IRuntimeModule& r)
	{
		return os << r.ToString();
	}
}