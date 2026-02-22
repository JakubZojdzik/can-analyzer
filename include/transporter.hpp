#pragma once
#include "can_message.hpp"
#include <ncurses.h>

class Transporter {
public:
    virtual ~Transporter() = default;
    virtual int receive(CANMessage& msg) = 0; // blocking, no timeout
    virtual int send(const CANMessage& msg) = 0;
};
