//  rabotaet na linuxe
//sudo g++ rangeM.cpp -o range-meters -lboost_system -lboost_serialization -pthread


#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <boost/asio.hpp>

const unsigned char PACKET_END_BYTE = 0xbe;  // End byte of the packet

void printPacket(const std::vector<unsigned char>& packet)
{
    /*
    // Print the received packet
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

int main()
{
    boost::asio::io_service io;
    boost::asio::serial_port serial(io, "/dev/ttyS0");  // Replace "COM3" with your serial port name
    serial.set_option(boost::asio::serial_port_base::baud_rate(9600));  // Set the baud rate

    // Byte string to send
    std::vector<unsigned char> byteString = { 0xae, 0xa7, 0x04, 0x00, 0x0e, 0x12, 0xbc, PACKET_END_BYTE };

    // Send the byte string
    boost::asio::write(serial, boost::asio::buffer(byteString));

    // Print the sent byte string
    std::cout << "Sent byte string: ";
    for (const auto& byte : byteString)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    std::vector<unsigned char> receivedPacket;

    std::cout << "Receiving data..."  <<   std::endl;

    while (true)
    {
        // Read data from the serial port
        char data;
        boost::asio::read(serial, boost::asio::buffer(&data, 1));

        //std::cout << "receiving smth..."  <<   std::endl;

        // Check for the end byte to mark the completion of a packet
        if (static_cast<unsigned char>(data) == PACKET_END_BYTE)
        {
            //std::cout << "checking stage..."  <<   std::endl;
            if (!receivedPacket.empty())
            {
                //std::cout << "printing..."  <<   std::endl;
                printPacket(receivedPacket);
                receivedPacket.clear();
            }
        }
        else
        {
            //std::cout << "pushing..."  <<   std::endl;
            receivedPacket.push_back(static_cast<unsigned char>(data));
        }
    }

    return 0;
}
