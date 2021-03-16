#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"
#include "Module/EventManager.h"
#include "Utils/Timer.h"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"

#ifdef _WIN32
#include <windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

#include <atomic>

class SerialPortModule : implements Rocket::IRuntimeModule
{
public:
    RUNTIME_MODULE_TYPE(SerialPortModule);
    SerialPortModule() = default;
    virtual ~SerialPortModule() = default;

    [[nodiscard]] int Initialize() final;
    void Finalize() final;
    void Tick(Timestep ts) final {}

    void MainLoop();
    bool OnWindowClose(Rocket::EventPtr& e);
private:
    std::atomic<bool> m_IsRunning = true;
    ElapseTimer m_Timer;
    itas109::CSerialPort m_SerialPort;
};

extern SerialPortModule* g_SerialPort;
