#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <termios.h>
#include "can_message.hpp"
#include "uart_transporter.hpp"

static void usage(const char *prog) {
    std::fprintf(stderr,
        "Usage: %s <device> <id> <data_hex>\n"
        "  device   - UART device path\n"
        "  id       - frame id in hex\n"
        "  data_hex - payload in hex\n",
        prog);
}

static bool parseHex(const char *hex, uint8_t *out, uint8_t &len) {
    size_t slen = std::strlen(hex);
    if (slen == 0 || slen % 2 != 0 || slen / 2 > 8)
        return false;

    len = slen / 2;
    for (uint8_t i = 0; i < len; i++) {
        char byte[3] = { hex[i*2], hex[i*2+1], '\0' };
        char *end;
        unsigned long val = std::strtoul(byte, &end, 16);
        if (*end != '\0')
            return false;
        out[i] = val & 0xff;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    char *port = argv[1];
    char *idStr = argv[2];
    char *dataStr = argv[3];

    char *end;
    uint32_t id = (uint32_t)std::strtoul(idStr, &end, 16);
    if (*end != 0) {
        std::fprintf(stderr, "Error: invalid CAN ID '%s'\n", idStr);
        return 1;
    }

    CANMessage msg;
    msg.identifier = id;
    msg.isRtr = false;
    msg.isExtd = (id > 0x7FF);

    if (!parseHex(dataStr, msg.data, msg.dlc)) {
        std::fprintf(stderr, "invalid data: '%s'\n", dataStr);
        return 1;
    }
    printf("id: %u\nextd: %u\n", msg.identifier, msg.isExtd);

    try {
        UARTTransporter transp(port, B921600);
        size_t sent = transp.send(msg);
        std::printf("Sent %zu bytes", sent);
    } catch (std::exception &e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }
    return 0;
}
