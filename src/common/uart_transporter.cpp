#include "uart_transporter.hpp"
#include "can_message.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <system_error>


UARTTransporter::UARTTransporter(const std::string& portName, int baudRate) {
    const int fd = open(portName.c_str(), O_RDWR | O_NOCTTY);
    if (fd < 0) {
        throw std::system_error(errno, std::generic_category(), "Failed to open " + portName);
    }

    struct termios dev{};
    if (tcgetattr(fd, &dev) != 0) {
        close(fd);
        throw std::system_error(errno, std::generic_category(), "tcgetattr failed");
    }

    cfsetispeed(&dev, baudRate);
    cfsetospeed(&dev, baudRate);

    dev.c_cflag |= (CLOCAL | CREAD);   // Enable receiver
    dev.c_cflag &= ~CSIZE;
    dev.c_cflag |= CS8;                // 8 bits
    dev.c_cflag &= ~PARENB;            // No parity
    dev.c_cflag &= ~CSTOPB;            // 1 stop bit
    dev.c_cflag &= ~CRTSCTS;           // No hardware flow control
    dev.c_lflag = 0;                   // Raw mode
    dev.c_oflag = 0;
    dev.c_iflag = 0;
    dev.c_cc[VMIN]  = 1;               // Blocking read
    dev.c_cc[VTIME] = 0;


    if (tcsetattr(fd, TCSANOW, &dev) != 0) {
        close(fd);
        throw std::system_error(errno, std::generic_category(), "tcsetattr failed");
    }

    serialFd = fd;
}

static void readExact(int fd, void* buffer, size_t size)
{
    uint8_t* ptr = static_cast<uint8_t*>(buffer);
    size_t total = 0;

    while (total < size) {
        ssize_t n = read(fd, ptr + total, size - total);

        if (n < 0) {
            if (errno == EINTR)
                continue;
            throw std::system_error(errno, std::generic_category(), "read failed");
        }

        if (n == 0) {
            throw std::runtime_error("Serial device closed");
        }

        total += n;
    }
}

static void syncMagicBytes(int fd) {
    uint8_t magic[2] = {0, 0};
    while(true) {
        read(fd, &magic[0], 1);
        if (magic[0] == 0xab) {
            read(fd, &magic[1], 1);
            if (magic[1] == 0xcd) {
                break;
            } else {
                magic[0] = magic[1];
            }
        }
    }
}

size_t UARTTransporter::receive(CANMessage& msg) {
    syncMagicBytes(serialFd);
    readExact(serialFd, &msg.timestamp, sizeof(msg.timestamp));
    readExact(serialFd, &msg.identifier, 4);

    uint8_t bits;
    readExact(serialFd, &bits, 1);
    msg.isRtr = bits & 1;
    msg.isExtd = (bits & 2) >> 1;

    readExact(serialFd, &msg.dlc, 1);
    msg.dlc &= 0xf;
    if (msg.dlc > 8)
        throw std::runtime_error("Invalid DLC");

    readExact(serialFd, msg.data, msg.dlc);
    return 16 + msg.dlc;
}

size_t UARTTransporter::send(CANMessage& msg) {
    uint8_t buf[16] = {0};
    size_t nbyte = serializeCanMessage(msg, buf);
    size_t res = write(serialFd, buf, nbyte);
    tcdrain(serialFd);
    return res;
}

UARTTransporter::~UARTTransporter() {
    if (serialFd >= 0) {
        tcdrain(serialFd);
        close(serialFd);
    }
}
