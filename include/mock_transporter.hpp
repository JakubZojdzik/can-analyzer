#pragma once
#include "transporter.hpp"
#include <vector>
#include <random>

class MockTransporter : public Transporter {
public:
    MockTransporter(int numIds, uint32_t isExtd);
    int receive(CANMessage& msg) override;
    int send(const CANMessage& msg) override;
private:
    std::vector<uint32_t> ids;
    std::discrete_distribution<> dist;
    std::mt19937 gen;
};
