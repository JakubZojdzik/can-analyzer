#include <cstdint>
#include <cstring>
#include <system_error>
#include "can_message.hpp"

size_t serializeCanMessage(CANMessage &msg, uint8_t *buffer) {
    buffer[0] = 0xab;
    buffer[1] = 0xbc;
    std::memcpy(&buffer[2], &msg.identifier, 4);
    buffer[6] = (msg.isRtr & 1) | ((msg.isExtd & 1) << 1);
    buffer[7] = msg.dlc;
    std::memcpy(&buffer[8], &msg.data, msg.dlc);
    return msg.dlc + 8;
}

void deserializeCanMessage(uint8_t *byteStream, CANMessage &msg) {
    if (byteStream[0] != 0xab || byteStream[1] != 0xcd) {
        throw std::runtime_error("Missing magic bytes");
    }

    std::memcpy(&msg.identifier, &byteStream[2], 4);

    msg.isRtr = byteStream[6] & 1;
    msg.isExtd = byteStream[6] & 2;

    msg.dlc = byteStream[7];
    if (msg.dlc > 8)
        throw std::runtime_error("Invalid DLC");

    std::memcpy(&msg.data, &byteStream[8], msg.dlc);
}
