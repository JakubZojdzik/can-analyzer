#include "../include/uart_transporter.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <system_error>
#include <stdint.h>

UARTTransporter::UARTTransporter(const std::string& portName, int baudRate) {
    int fd = open(portName.c_str(), O_RDWR | O_NOCTTY);
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

    tcflush(fd, TCIFLUSH);

    if (tcsetattr(fd, TCSANOW, &dev) != 0) {
        close(fd);
        throw std::system_error(errno, std::generic_category(), "tcsetattr failed");
    }

    this->serialFd = fd;
}

bool read_exact(int fd, uint8_t* buffer, size_t size)
{
    size_t total = 0;
    while (total < size) {
        ssize_t n = read(fd, buffer + total, size - total);
        if (n <= 0)
            return false;
        total += n;
    }
    return true;
}

int UARTTransporter::receive(CANMessage& msg) {
    size_t received = 0;
    msg.identifier = 0;
    if (read(this->serialFd, &msg.identifier, 4) != 4)
        throw std::system_error(errno, std::generic_category(), "read failed");
    uint8_t bits;
    if (read(this->serialFd, &bits, 1) != 1)
        throw std::system_error(errno, std::generic_category(), "read failed");
    msg.isRtr = bits & 1;
    msg.isExtd = bits & 2;
    msg.isSelf = bits & 4;
    if (read(this->serialFd, &msg.dlc, 1) != 1)
        throw std::system_error(errno, std::generic_category(), "read failed");
    if (read(this->serialFd, &msg.data, msg.dlc) != msg.dlc)
        throw std::system_error(errno, std::generic_category(), "read failed");
    return 6+msg.dlc;
}

int UARTTransporter::send(const CANMessage& msg) {
    return -1;
}

UARTTransporter::~UARTTransporter() {
    if (serialFd >= 0) close(serialFd);
}
