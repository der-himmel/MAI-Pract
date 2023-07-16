//Взятие данных с дальномера

#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <boost/asio.hpp>
#include <windows.h>

const unsigned char PACKET_END_BYTE = 0xBE;  // End byte of the packet

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
        std::cout << " --- " << static_cast<float>(static_cast<unsigned char>(hexChar)) * 0.1 << " meters." << system("cls");
    }
}

int main()
{
    boost::asio::io_service io;
    boost::asio::serial_port serial(io, "COM3");  // Replace "COM3" with your serial port name
    serial.set_option(boost::asio::serial_port_base::baud_rate(9600));  // Set the baud rate

    // Byte string to send
    std::vector<unsigned char> byteString = { 0xAE, 0xA7, 0x04, 0x00, 0x0E, 0x12, 0xBC, PACKET_END_BYTE };

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

    while (true)
    {
        // Read data from the serial port
        char data;
        boost::asio::read(serial, boost::asio::buffer(&data, 1));

        // Check for the end byte to mark the completion of a packet
        if (static_cast<unsigned char>(data) == PACKET_END_BYTE)
        {
            if (!receivedPacket.empty())
            {
                printPacket(receivedPacket);
                receivedPacket.clear();
            }
        }
        else
        {
            receivedPacket.push_back(static_cast<unsigned char>(data));
        }
    }

    return 0;
}
