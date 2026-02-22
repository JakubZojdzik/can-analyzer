#include <cstdint>
#include <termios.h>
#include "../include/mock_transporter.hpp"

int main() {
    uint32_t baudRate = B921600;

    MockTransporter transp(100, true);
    CANMessage msg;
    while(true) {
        transp.receive(msg);
        printf("[%02x] ", msg.identifier);
        for (int i = 0; i < msg.dlc; i++) {
            printf("%02x ", msg.data[i]);
        }
        printf("\n");
    }
}
