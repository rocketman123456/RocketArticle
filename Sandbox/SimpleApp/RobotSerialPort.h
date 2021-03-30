#pragma once
#include "Core/Core.h"
#include "Interface/IRuntimeModule.h"
#include "Event/Event.h"
#include "Utils/Timer.h"
#include "Utils/Variant.h"
#include "Utils/Semaphore.h"

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"

#include <atomic>
#include <mutex>

class ReadSlot : public has_slots<>
{
private:
    ReadSlot() = default;
public:
    ReadSlot(itas109::CSerialPort* sp) : rec_len_(-1), sp_(sp) {}

    void OnReadMessage();

private:
    itas109::CSerialPort* sp_;
    uint8_t get_data_[1024];
    int rec_len_;
    int count_read_ = 0;
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

    void OpenPort();

    void MainLoop();
    bool OnWindowClose(Rocket::EventPtr& e);
    bool OnAction(Rocket::EventPtr& e);
    bool OnSendData(Rocket::EventPtr& e);

    Rocket::Ref<ReadSlot> GetReadSlot() { return read_slot_; }
    itas109::CSerialPort& GetSerialPort() { return serial_port_; }

private:
    Rocket::Ref<ReadSlot> read_slot_;
    std::atomic<bool> is_running_ = true;
    ElapseTimer timer_;
    itas109::CSerialPort serial_port_;
    Rocket::Queue<Rocket::EventVarVec> vars_;
    bool use_fake_data_ = false;
    std::mutex lock_;

    float motor_data[10] = {0};
    float imu_data[3] = {0};
};

extern SerialPortModule* g_SerialPort;
