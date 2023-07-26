#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <boost/asio.hpp>
#include "RangeF.h"

RangeF::RangeF()
{
    std::cout << "Rangefinder connected successfully" << std::endl;
}

void RangeF::printPacket(const std::vector<unsigned char>& packet)
{
    /*
    std::cout << "Received packet: ";
    for (const auto& byte : packet)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    */

    if (packet.size() >= 7 && packet[4] == 0x85)
    {
        char hexChar = packet[8];
        std::cout << static_cast<float>(static_cast<unsigned char>(hexChar)) * 0.1 << " meters." << std::endl << std::endl;
    }
}

void RangeF::sendRequest()
{
    /*boost::asio::io_service io;
    boost::asio::serial_port serial(io, "/dev/ttyS0");
    serial.set_option(boost::asio::serial_port_base::baud_rate(9600));*/

    std::vector<unsigned char> byteString = { 0xae, 0xa7, 0x04, 0x00, 0x05, 0x09, 0xbc, PACKET_END_BYTE }; //singe measurement
    //std::vector<unsigned char> byteString = { 0xae, 0xa7, 0x04, 0x00, 0x0e, 0x12, 0xbc, PACKET_END_BYTE }; //continuous measurement

    //boost::asio::write(serial, boost::asio::buffer(byteString));

    /*std::cout << "Sent byte string: ";
    for (const auto& byte : byteString)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;*/
}

float RangeF::getDist()
{
    sendRequest();

    //std::cout << "Receiving data..." << std::endl;

    //std::cout << "distance" << std::endl;
    dist = 10.8;

    /*while (true)
    {
        char data;
        boost::asio::read(serial, boost::asio::buffer(&data, 1));

        //std::cout << "receiving smth..."  <<   std::endl;
        if (static_cast<unsigned char>(data) == PACKET_END_BYTE)
        {
            //std::cout << "checking stage..."  <<   std::endl;
            if (!receivedPacket.empty())
            {
                //std::cout << "printing..."  <<   std::endl
                printPacket(receivedPacket);
                receivedPacket.clear();
                break;
            }
        }
        else
        {
            //std::cout << "pushing..."  <<   std::endl;
            receivedPacket.push_back(static_cast<unsigned char>(data));
        }
    }*/

    return dist;
}
