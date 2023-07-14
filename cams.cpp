#include <opencv2/opencv.hpp>

int main() {
    // Create a VideoCapture object
    	cv::VideoCapture ipcam;
	cv::VideoCapture therm(0);

    // Open the RTSP stream
	ipcam.open("rtsp://admin:Admin123@172.16.0.5:554/12");

    // Check if the stream is opened successfully
    	if (!ipcam.isOpened()) 
	{
        	std::cout << "Failed to open the RTSP stream." << std::endl;
        	return -1;
    	}
	
	if (!therm.isOpened())
	{
		std::cout << "Failed to load thermal output." << std::endl;
		return -1;
	}


    // Main loop to retrieve and display frames
    while (true) 
	{
        cv::Mat ipframe, thermal, color, res;

        // Read a frame from the RTSP stream
        if (!ipcam.read(ipframe)) 
	{
            	std::cout << "Failed to read frame from the RTSP stream." << std::endl;
            	break;
        }

	if (!therm.read(thermal))
	{
		std::cout << "Failed to read frame from the thermal camera." << std::endl;
		break;
	} 
	
	cv::resize(thermal,thermal,  ipframe.size());	

	cv::applyColorMap(thermal, color,cv::COLORMAP_JET);

	cv::hconcat(color, ipframe, res);

	//cv::imshow("RTSP", ipframe);

        //cv::imshow("Thermal", color);

	cv::imshow("IMAGE", res);

        // Check for user input to exit
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the VideoCapture object
    	ipcam.release();
	therm.release();

    // Close all OpenCV windows
    cv::destroyAllWindows();

    return 0;
}
