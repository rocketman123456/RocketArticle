#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"
#include "Event/Event.h"
#include "Utils/Timer.h"
#include "Utils/Variant.h"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"

#include <atomic>

class ReadSlot : public has_slots<>
{
public:
    ReadSlot(itas109::CSerialPort * sp) : recLen(-1), p_sp(sp) {}

	void OnReadMessage()
	{
		//read
        recLen = p_sp->readAllData(str);

		if(recLen > 0)
		{
			countRead++;
			str[recLen] = '\0';
            RK_CORE_TRACE("receive data : {}, receive data : {}, receive count : {}", str, recLen, countRead);
		}
	};

private:
	ReadSlot() = default;

private:
    itas109::CSerialPort* p_sp;
	char str[1024];
	int recLen;
    int countRead = 0;
};

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
    bool OnAction(Rocket::EventPtr& e);
    bool OnMotor(Rocket::EventPtr& e);
private:
    std::atomic<bool> m_IsRunning = true;
    ElapseTimer m_Timer;
    itas109::CSerialPort m_SerialPort;
    Rocket::Queue<Rocket::EventVarVec> m_Vars;
    bool m_UseFakeData = false;
};

extern SerialPortModule* g_SerialPort;
