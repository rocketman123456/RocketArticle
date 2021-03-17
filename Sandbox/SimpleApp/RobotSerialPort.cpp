#include "RobotSerialPort.h"
#include "Module/EventManager.h"

using namespace Rocket;
using namespace itas109;

int SerialPortModule::Initialize()
{
    m_Timer.Start();
    m_Timer.MarkLapping();
    RK_CORE_INFO("Version : {}", m_SerialPort.getVersion());
    Vec<SerialPortInfo> availablePortsList = CSerialPortInfo::availablePortInfos();

	if (availablePortsList.size() == 0)
	{
        RK_CORE_INFO("No valid port");
	}
    else
    {
        RK_CORE_INFO("availableFriendlyPorts : ");
		
        for (int i = 0; i < availablePortsList.size(); i++)
        {
            RK_CORE_INFO("{} - {} {}", i, availablePortsList[i].portName, availablePortsList[i].description);
        }

        String portName = availablePortsList[0].portName;
        m_SerialPort.init(portName, 9600, Parity::ParityOdd, DataBits::DataBits8, StopBits::StopOne);
        m_SerialPort.open();
		
		if(m_SerialPort.isOpened())
			RK_CORE_INFO("open {} success", portName);	
		else
			RK_CORE_INFO("open {} failed", portName);
    }
    return 0;
}

void SerialPortModule::Finalize()
{
    if(m_SerialPort.isOpened())
    {
        m_SerialPort.close();
    }
    m_Timer.Stop();
}

bool SerialPortModule::OnWindowClose(EventPtr& e)
{
    m_IsRunning = false;
    return false;
}

bool SerialPortModule::OnAction(EventPtr& e)
{
    static char str[1024];
    RK_CORE_TRACE("Send Data Through Serial Port");
    str[0] = 'H';str[1] = 'e';str[2] = 'l';str[3] = 'l';str[4] = 'o';str[5] = '\0';
    m_SerialPort.writeData(str, 5);
    return false;
}

void SerialPortModule::MainLoop()
{
    while(m_IsRunning)
    {
        double elapsed = m_Timer.GetElapsedTime();
        if(elapsed >= 1000)
        {
            m_Timer.MarkLapping();
            EventVarVec var;
            var.resize(2 + 10 + 10);

            // Event Type
            var[0].type = Variant::TYPE_STRING_ID;
            var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_response");
            // Action
            var[1].type = Variant::TYPE_STRING_ID;
            var[1].asStringId = 0;

            EventPtr event = CreateRef<Event>(var);
            g_EventManager->TriggerEvent(event);
        }
    }
}
