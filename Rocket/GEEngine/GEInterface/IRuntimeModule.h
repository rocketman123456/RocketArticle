#pragma once
#include "GECore/Core.h"
#include "GEEvent/Event.h"
#include "GEUtils/Timestep.h"

namespace Rocket {
    Interface IRuntimeModule{
    public:
        IRuntimeModule(const std::string& name = "IRuntimeModule") : m_Name(name) {}
        virtual ~IRuntimeModule() {};

        virtual int Initialize() = 0;
        virtual void Finalize() = 0;

        virtual void Tick(Timestep ts) = 0;
        virtual void OnEvent(Event& event) {}

        // For debug
        const std::string& GetName() const { return m_Name; }
    private:
        std::string m_Name;
    };
}
