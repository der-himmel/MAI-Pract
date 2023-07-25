#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include "Contr.h"
#include "RangeF.h"
#include "Cam.h"

Contr::Contr(Cam* cam, RangeF* range)
{
	//this->flag = 1;
	//this->status = 0;
	this->cam = cam;
	this->range = range;
};

void Contr::operating()
{
	while (1)
	{
		recording();
	}
}

void Contr::recording()
{
	cam->update();
	range->getDist();

	distance = std::to_string(range->dist);

	cv::putText(cam->res, distance, cv::Point(30, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
	//cv::imshow("RTSP stream + Thermal output", cam->res);

	bool success = cv::imwrite(cam->filename, cam->res);

	if (success)
	{
		std::cout << "Image saved as " << cam->filename << std::endl;
	}
	else
	{
		std::cout << "Error: Image not saved." << std::endl;
	}

	open = cv::imread(cam->filename);
	cv::imshow("Image", open);
}
