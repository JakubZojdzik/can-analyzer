#pragma once
#include <cstdint>

struct CANMessage {
    unsigned long identifier;
    bool isRtr;
    bool isExtd;
    uint8_t dlc;
    uint8_t data[8]; // may be less, depends on dlc
};

std::size_t serializeCanMessage(CANMessage &msg, uint8_t *buffer);
void deserializeCanMessage(uint8_t *byteStream, CANMessage &msg);

