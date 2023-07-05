#pragma ide diagnostic ignored "EndlessLoop"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void main() {
    VideoCapture cap(2);

    while (true) {
        Mat frame;
        Mat frameHSV, color, res;
        bool bSuccess = cap.read(frame);

        if (bSuccess == false)
        {
            cout << "Found the end of the video" << endl;
            break;
        }

        applyColorMap(frame, color, COLORMAP_JET);

        hconcat(frame, color, res);

        imshow("Thermal Output", res);
        waitKey(1);
    }
}
