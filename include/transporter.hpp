#pragma once
#include "can_message.hpp"
#include <ncurses.h>

class Transporter {
public:
    virtual ~Transporter() = default;
    virtual size_t receive(CANMessage& msg) = 0; // blocking, no timeout
    virtual size_t send(CANMessage& msg) = 0;
};
