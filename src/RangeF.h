#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <boost/asio.hpp>

class RangeF
{
private:
    std::vector<unsigned char> receivedPacket;
    const unsigned char PACKET_END_BYTE = 0xbe;

public:
    float dist;

    RangeF();
    void printPacket(const std::vector<unsigned char>& packet);
    void sendRequest();
    float getDist();
    ~RangeF();
};
