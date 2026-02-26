#pragma once
#include "transporter.hpp"
#include <string>

class UARTTransporter : public Transporter {
public:
    UARTTransporter(const std::string& portName, int baudRate);
    ~UARTTransporter();
    size_t receive(CANMessage& msg) override;
    size_t send(CANMessage& msg) override;
private:
    int serialFd = -1;
};
