#pragma once
#include "Core/Core.h"
#include "Utils/Timestep.h"

namespace Rocket
{
    Interface IRuntimeModule
    {
    public:
        IRuntimeModule(const std::string &name = "IRuntimeModule") : m_Name(name) {}
        virtual ~IRuntimeModule() = default;

        virtual int Initialize() = 0;
        virtual void Finalize() = 0;

        virtual void Tick(Timestep ts) = 0;

        // For Debug
        [[nodiscard]] const std::string &GetName() const { return m_Name; }
    private:
        std::string m_Name;
    };
}