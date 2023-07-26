#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include "RangeF.h"
#include "Cam.h"

class Contr
{
private:
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket clientSocket;

	std::string distance;
	cv::Mat open;

	RangeF* range;
	Cam* cam;

public:
	Contr(Cam* cam, RangeF* range);
	void operating();
	void recording();
};
