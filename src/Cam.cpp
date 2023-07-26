#include <opencv2/opencv.hpp>
#include <cstdio>
#include "Cam.h"

Cam::Cam() : therm(2), ipcam(1)
{
    //cv::VideoCapture therm(0);
    //cv::VideoCapture ipcam;

    //cv::VideoCapture ipcam(1);

    //ipcam.open("rtsp://admin:Admin123@172.16.0.5:554/12");

    therm.set(cv::CAP_PROP_FPS, 5);
    ipcam.set(cv::CAP_PROP_FPS, 5);

    if (!this->ipcam.isOpened())
    {
        std::cout << "Error! Failed to load RTSP stream..." << std::endl;
    }
    else
    {
        std::cout << "RTSP stream is opened successfully." << std::endl;
    }

    if (!this->therm.isOpened())
    {
        std::cout << "Error! Failed to load thermal output..." << std::endl;
    }
    else
    {
        std::cout << "Thermal output is opened successfully." << std::endl;
    }
}

cv::Mat Cam::getIpFrame()
{
    ipcam.read(ipframe);

    return ipframe;
}

cv::Mat Cam::getThermalFrame()
{
    therm.read(thermal);

    return thermal;
}

void Cam::update()
{
    ipframe = getIpFrame();
    thermal = getThermalFrame();

    cv::resize(thermal, thermal, ipframe.size());
    //cv::applyColorMap(thermal, color, cv::COLORMAP_JET);
    cv::hconcat(thermal, ipframe, res);

    //cv::imshow("RTSP stream + Thermal output", res);
    //cv::waitKey(1);
}
