#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/asio.hpp>

int main()
{
    boost::asio::io_service io;
    boost::asio::serial_port serial(io, "/dev/ttyUSB0");  // Replace "/dev/ttyUSB0" with your serial port name
    serial.set_option(boost::asio::serial_port_base::baud_rate(9600));  // Set the baud rate

    std::vector<unsigned char> byteString = {0xAE, 0xA7, 0x04, 0x00, 0x0E, 0x12, 0xBC, 0xBE};

    // Send the byte string
    boost::asio::write(serial, boost::asio::buffer(byteString));
    
    // Print the sent byte string
    std::cout << "Sent byte string: ";
    for (const auto& byte : byteString)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
	std::cout << "Received bytes: " << std::endl;

    std::vector<unsigned char> receivedData(1024);  // Buffer to hold received data

    while (true)
    {
        // Read data from the serial port
        size_t bytesRead = serial.read_some(boost::asio::buffer(receivedData));

        // Process the received data
        for (size_t i = 0; i < bytesRead; ++i)
        {
            // Print the received byte as hexadecimal
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(receivedData[i]) << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
