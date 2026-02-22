#pragma once
#include <chrono>
#include "can_message.hpp"

struct DisplayRecord {
    CANMessage msg;
    std::chrono::steady_clock::time_point timestamp;
    unsigned long timeDelta;
    uint8_t highlightBitmap;
};
