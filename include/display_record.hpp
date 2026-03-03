#pragma once
#include <cstdint>
#include <chrono>
#include "can_message.hpp"

struct DisplayRecord {
    CANMessage msg;
    std::chrono::steady_clock::time_point timestamp;
    uint32_t timeDelta;
    uint8_t highlightBitmap;
};

struct RecordChange {
    unsigned int position;
    bool isNew;
};
