#include "RobotSerialPort.h"
#include "Module/EventManager.h"
#include "Module/Application.h"

using namespace Rocket;
using namespace itas109;

#ifdef _WIN32
#include <windows.h>
#define imsleep(microsecond) Sleep(microsecond) // ms
#else
#include <unistd.h>
#define imsleep(microsecond) usleep(1000 * microsecond) // ms
#endif

static FastSemaphore g_motor_sem;
static std::mutex g_mutex;
static const int32_t delay_ms = 8;
static std::atomic<bool> g_wait_data;

static void sleep_high_res(int32_t count_ms)
{
    bool sleep = true;
    auto start = std::chrono::system_clock::now();
    while(sleep)
    {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        if ( elapsed.count() > count_ms )
            sleep = false;
    }

    //imsleep(count_ms);
}

static void CRC16_MODBUS(uint8_t input[], int size, uint8_t* low_value, uint8_t* high_value)
{
    uint16_t crc = 0xffff;
    for (int n = 0; n < size; n++)
    {
        crc = input[n] ^ crc;
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x01)
            {
                crc = crc >> 1;
                crc = crc ^ 0xa001;
            }
            else
            {
                crc = crc >> 1;
            }
        }
    }
    *low_value = crc & 0xFF;
    *high_value = (uint8_t)(crc >> 8);
}

void ReadSlot::OnReadMessage()
{
    //read
    rec_len_ = sp_->readAllData((char*)get_data_);

    if(rec_len_ >= 8)
    {
        count_read_++;
        //RK_CORE_INFO("receive data len : {}, receive count : {}", rec_len_, count_read_);
        rec_len_ = -1;
    }

    if((rec_len_ % 8) == 0)
    {
        int curr_len = 0;
        while(curr_len < rec_len_)
        {
            EventVarVec var;
            var.resize(2 + 2);

            var[0].type = Variant::TYPE_STRING_ID;
            var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_response");

            // id/type
            var[1].type = Variant::TYPE_UINT32;
            var[1].asUInt32 = get_data_[curr_len + 0];

            // cmd
            var[2].type = Variant::TYPE_UINT32;
            var[2].asUInt32 = get_data_[curr_len + 1];

            float data;

            memcpy(&data, &get_data_[curr_len + 2], sizeof(float));
            var[3].type = Variant::TYPE_FLOAT;
            var[3].asFloat = data;

            EventPtr event = CreateRef<Event>(var);
            if(get_data_[curr_len + 1] == 0x09)    // motor data
                g_EventManager->TriggerEvent(event);
            else if(get_data_[curr_len + 1] == 0x08)   // imu data
                g_EventManager->TriggerEvent(event);

            curr_len += 8;
        }
    }
    else
    {
        EventVarVec var;
        var.resize(2 + 2);

        var[0].type = Variant::TYPE_STRING_ID;
        var[0].asStringId = GlobalHashTable::HashString("Event"_hash, "ui_event_response");

        // id/type
        var[1].type = Variant::TYPE_UINT32;
        var[1].asUInt32 = get_data_[0];

        // cmd
        var[2].type = Variant::TYPE_UINT32;
        var[2].asUInt32 = get_data_[1];

        float data;

        memcpy(&data, &get_data_[2], sizeof(float));
        var[3].type = Variant::TYPE_FLOAT;
        var[3].asFloat = data;

        EventPtr event = CreateRef<Event>(var);
        if(get_data_[1] == 0x09)    // motor data
            g_EventManager->TriggerEvent(event);
        else if(get_data_[1] == 0x08)   // imu data
            g_EventManager->TriggerEvent(event);

        g_wait_data = false;
    }
}

int SerialPortModule::Initialize()
{
    RK_CORE_INFO("Serial Version : {}", serial_port_.getVersion());
    return 0;
}

void SerialPortModule::OpenPort()
{
    Vec<SerialPortInfo> availablePortsList = CSerialPortInfo::availablePortInfos();

	if (availablePortsList.size() == 0)
	{
        RK_CORE_INFO("No valid port");
	}
    else
    {
        RK_CORE_INFO("availableFriendlyPorts : {}", availablePortsList.size());
		
        for (int i = 0; i < availablePortsList.size(); i++)
        {
            RK_CORE_INFO("{} - {} {}", i, availablePortsList[i].portName, availablePortsList[i].description);
        }
        auto& config = g_Application->GetConfig();
        uint32_t port_num = config->GetConfigInfo<uint32_t>("SerialPort", "port-num");
        uint32_t baud_rate = config->GetConfigInfo<uint32_t>("SerialPort", "baud-rate");
        uint32_t parity = config->GetConfigInfo<uint32_t>("SerialPort", "parity");
        
        if(port_num > availablePortsList.size() - 1)
            port_num = availablePortsList.size() - 1;

        String portName = availablePortsList[port_num].portName;
        serial_port_.init(portName, baud_rate, (Parity)parity, DataBits::DataBits8, StopBits::StopOne);
        serial_port_.open();

        use_fake_data_ = true;//!serial_port_.isOpened();
		if(serial_port_.isOpened())
			RK_CORE_INFO("open {} success", portName);	
		else
			RK_CORE_INFO("open {} failed", portName);

        read_slot_ = CreateRef<ReadSlot>(&serial_port_);
        serial_port_.readReady.connect(read_slot_.get(), &ReadSlot::OnReadMessage);
    }

    timer_.Start();
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

void SerialPortModule::DelayMs(uint32_t ms)
{
    timer_.MarkLapping();
    double elapsed = 0.0;
    while (elapsed < ms)
        elapsed = timer_.GetElapsedTime();
}

void SerialPortModule::SendData(uint8_t* data, uint32_t len)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    serial_port_.writeData((char*)data, len);
    sleep_high_res(delay_ms);
}

bool SerialPortModule::OnAction(EventPtr& e)
{
    lock_.lock();
    vars_.emplace(e->variable.begin(), e->variable.end());
    lock_.unlock();
    RK_CORE_TRACE("Send Control Data");
    static uint8_t str[128];

    for(int i = 0; i < 10; ++i)
    {
        uint8_t data[8] = {0};
        data[0] = i + 1;
        data[1] = 0x04;
        memcpy(&data[2], &e->variable[2 + i].asFloat, sizeof(float));
        CRC16_MODBUS(data, 6, &data[6], &data[7]);
        SendData(data, 8);
    }

    uint8_t data[8] = {0};
    data[0] = 0x00;
    data[1] = 0x06;
    data[2] = e->variable[12].asUInt32 >> 8;
    data[3] = e->variable[12].asUInt32 & 0xff;
    CRC16_MODBUS(data, 6, &data[6], &data[7]);
    SendData(data, 8);

    return false;
}

bool SerialPortModule::OnSendData(Rocket::EventPtr& e)
{
    uint32_t control_id = e->variable[1].asUInt32;
    uint32_t command = e->variable[2].asUInt32;
    uint8_t data[8] = {0};
    data[0] = control_id;
    data[1] = command;
    if(command == 0x01)
    {
        memcpy(&data[2], &e->variable[3].asUInt32, sizeof(uint32_t));
        CRC16_MODBUS(data, 6, &data[6], &data[7]);
        SendData(data, 8);
    }
    else if(command == 0x02)
    {
        memcpy(&data[2], &e->variable[3].asInt32, sizeof(int32_t));
        CRC16_MODBUS(data, 6, &data[6], &data[7]);
        SendData(data, 8);
    }
    else if(command == 0x03)
    {
        CRC16_MODBUS(data, 6, &data[6], &data[7]);
        SendData(data, 8);
    }
    else if(command == 0x04)
    {
        
        memcpy(&data[2], &e->variable[3].asFloat, sizeof(float));
        CRC16_MODBUS(data, 6, &data[6], &data[7]);
        SendData(data, 8);
    }
    else if(command == 0x05)
    {
        CRC16_MODBUS(data, 6, &data[6], &data[7]);
        SendData(data, 8);
    }
    return false;
}

void SerialPortModule::MainLoop()
{
    //double elapsed = 0.0;

    while(is_running_)
    {
        uint8_t data[8] = {0};

        // get motor data
        for(int i = 0; i < 10; ++i)
        {
            g_wait_data = false;
            data[0] = i + 1;
            data[1] = 0x05;
            CRC16_MODBUS(data, 6, &data[6], &data[7]);
            SendData(data, 8);
            //while(!g_wait_data) {}
        }

        sleep_high_res(delay_ms);

        // get imu data
        for(int i = 0; i < 3; ++i)
        {
            data[0] = 0x11 + i;
            data[1] = 0x07;
            CRC16_MODBUS(data, 6, &data[6], &data[7]);
            SendData(data, 8);
        }
    }
}
