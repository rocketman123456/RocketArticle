#include "RobotSerialPort.h"
#include "Module/EventManager.h"
#include "Module/Application.h"

using namespace Rocket;
using namespace itas109;

int SerialPortModule::Initialize()
{
    RK_CORE_INFO("Version : {}", serial_port_.getVersion());
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
        auto& config = g_Application->GetConfig();
        uint32_t port_num = config->GetConfigInfo<uint32_t>("SerialPort", "port-num");
        uint32_t baud_rate = config->GetConfigInfo<uint32_t>("SerialPort", "baud-rate");
        
        String portName = availablePortsList[port_num].portName;
        serial_port_.init(portName, baud_rate, Parity::ParityNone, DataBits::DataBits8, StopBits::StopOne);
        //serial_port_.open();

        use_fake_data_ = true;//!serial_port_.isOpened();
		if(serial_port_.isOpened())
			RK_CORE_INFO("open {} success", portName);	
		else
			RK_CORE_INFO("open {} failed", portName);
    }

    timer_.Start();
    return 0;
}

void SerialPortModule::Finalize()
{
    if(serial_port_.isOpened())
    {
        serial_port_.close();
    }
    timer_.Stop();
}

bool SerialPortModule::OnWindowClose(EventPtr& e)
{
    is_running_ = false;
    return false;
}

bool SerialPortModule::OnAction(EventPtr& e)
{
    lock_.lock();
    //vars_.push(e->Var);
    vars_.emplace(e->variable.begin(), e->variable.end());
    lock_.unlock();
    RK_CORE_TRACE("Send Control Data");
    static char str[1024];
    str[0] = 'H';str[1] = 'e';str[2] = 'l';str[3] = 'l';str[4] = 'o';str[5] = '\0';
    serial_port_.writeData(str, 5);
    return false;
}

bool SerialPortModule::OnMotor(Rocket::EventPtr& e)
{
    uint32_t motor_id = e->variable[1].asUInt32;
    uint32_t command = e->variable[2].asUInt32;
    uint32_t data = e->variable[3].asFloat;
    RK_CORE_TRACE("Send Motor Data");
    static char str[1024];
    str[0] = 'H';str[1] = 'e';str[2] = 'l';str[3] = 'l';str[4] = 'o';str[5] = '\0';
    serial_port_.writeData(str, 5);
    return false;
}

void SerialPortModule::MainLoop()
{
    while(is_running_)
    {
        double elapsed = timer_.GetElapsedTime();
        if(elapsed >= 50)
        {
            timer_.MarkLapping();
            EventVarVec var;
            var.resize(2 + 10 + 10);
            
            if(use_fake_data_)
            {
                lock_.lock();
                RK_CORE_TRACE("Vars Queue Size {}", vars_.size());
                if(vars_.size() > 0)
                {
                    auto top = vars_.front();
                    var.assign(top.begin(), top.end());
                    if(vars_.size() > 1)
                        vars_.pop();
                }
                lock_.unlock();
            }
            else
            {

            }

            // Event Type
            var[0].type = Variant::TYPE_STRING_ID;
            var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_response");

            EventPtr event = CreateRef<Event>(var);
            g_EventManager->TriggerEvent(event);
        }
    }
}
