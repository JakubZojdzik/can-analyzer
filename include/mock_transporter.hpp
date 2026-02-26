#pragma once
#include "transporter.hpp"
#include <vector>
#include <random>

class MockTransporter : public Transporter {
public:
    MockTransporter(int numIds, unsigned long isExtd);
    size_t receive(CANMessage& msg) override;
    size_t send(CANMessage& msg) override;
private:
    std::vector<unsigned long> ids;
    std::discrete_distribution<> dist;
    std::mt19937 gen;
};
