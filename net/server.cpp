#include <iostream>
#include <boost/asio.hpp> // Include the Boost.Asio header
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

using namespace std;
using namespace boost::asio;

int main() {
    io_service ioService;
    ip::tcp::acceptor acceptor(ioService, ip::tcp::endpoint(ip::tcp::v4(), 11111));
    ip::tcp::socket clientSocket(ioService);
    acceptor.accept(clientSocket);

    cout << "Established connection with: " << clientSocket.remote_endpoint() << endl;

    cv::VideoCapture vid(0);
    if (!vid.isOpened()) {
        cout << "Failed to open the camera." << endl;
        return 1;
    }

    while (true) {
        cv::Mat frame;
        vid.read(frame);
        cout << "Reading frames..." << endl;

        // Encode the frame as JPEG before sending
        std::vector<uchar> encoded;
        cv::imencode(".jpg", frame, encoded);

        // Send frame size first
        uint64_t msg_size = encoded.size();
        clientSocket.write_some(boost::asio::buffer(&msg_size, sizeof(msg_size)));

        // Send the serialized frame data to the client
        clientSocket.write_some(boost::asio::buffer(encoded.data(), encoded.size()));

        cv::imshow("SOURCE", frame);
        char key = cv::waitKey(1);
        if (key == 'q') {
            clientSocket.close();
            break;
        }
    }

    acceptor.close();
    return 0;
}
