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

	std::string cameraIpAddress = "172.16.0.5";
	std::string cameraPort = "80";

	std::string username = "admin";
	std::string password = "Admin123";

public:
	Contr(Cam* cam, RangeF* range);
	void operating();
	void recording();
	void listening();
};
