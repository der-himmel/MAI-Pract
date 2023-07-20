//Подключение к камере через RTSP

#include <opencv2/opencv.hpp>

int main() {
    // Create a VideoCapture object
    cv::VideoCapture cap;

    // Open the RTSP stream
    cap.open("rtsp://<camera_ip_address>/stream");

    // Check if the stream is opened successfully
    if (!cap.isOpened()) {
        std::cout << "Failed to open the RTSP stream." << std::endl;
        return -1;
    }

    // Main loop to retrieve and display frames
    while (true) {
        cv::Mat frame;

        // Read a frame from the RTSP stream
        if (!cap.read(frame)) {
            std::cout << "Failed to read frame from the RTSP stream." << std::endl;
            break;
        }

        // Display the frame
        cv::imshow("RTSP Stream", frame);

        // Check for user input to exit
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the VideoCapture object
    cap.release();

    // Close all OpenCV windows
    cv::destroyAllWindows();

    return 0;
}
