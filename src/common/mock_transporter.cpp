#include "mock_transporter.hpp"
#include <ctime>
#include <thread>
#include <chrono>

MockTransporter::MockTransporter(int numIds, unsigned long isExtd) {
    srand(time(0));
    std::vector<int> weights;
    const uint32_t idMask = 0x7ff | ((uint32_t)-isExtd & 0x1ffff800);
    for (int i = 0; i < numIds; i++) {
        ids.push_back(rand() & idMask);
        weights.push_back(numIds - i);
    }

    std::random_device rd;
    gen = std::mt19937(rd());
    dist = std::discrete_distribution<>(weights.begin(), weights.end());
}

size_t MockTransporter::receive(CANMessage& msg) {
    msg.identifier = ids[dist(gen)];
    int bits = random();
    msg.isRtr = bits & 1;
    msg.isExtd = (bits & 2) >> 1;
    msg.dlc = (random() % 8) + 1;
    for (int i = 0; i < msg.dlc; i++) {
        msg.data[i] = random() & 0xff;
    }
    std::chrono::milliseconds timespan(random()%100);
    std::this_thread::sleep_for(timespan);
    return 6+msg.dlc;
}

size_t MockTransporter::send(CANMessage& msg) {
    return -1;
}

