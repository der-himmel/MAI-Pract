#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/mat.hpp"

class Cam
{
private:
    cv::Mat ipframe, thermal;
    cv::Mat color;

    cv::VideoCapture therm;
    cv::VideoCapture ipcam;

    cv::Mat getIpFrame();
    cv::Mat getThermalFrame();

    void captureImage();
    void removeImage();

public:
    std::string filename = "frame.jpg";
    cv::Mat res;

    Cam();
    void update();
    ~Cam();
};
