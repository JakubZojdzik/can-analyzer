#pragma once

#include <cstdint>

struct CANMessage {
    uint32_t identifier;
    bool isRtr;
    bool isExtd;
    bool isSelf;
    uint8_t dlc;
    uint8_t data[8]; // may be less, depends on dlc
};
