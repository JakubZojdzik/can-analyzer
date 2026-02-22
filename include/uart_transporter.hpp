#pragma once
#include "transporter.hpp"
#include <string>

class UARTTransporter : public Transporter {
public:
    UARTTransporter(const std::string& portName, int baudRate);
    ~UARTTransporter();
    int receive(CANMessage& msg) override;
    int send(const CANMessage& msg) override;
private:
    int serialFd = -1;
};
