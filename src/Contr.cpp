#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include "Contr.h"
#include "RangeF.h"
#include "Cam.h"

using namespace std;
using namespace boost::asio;

Contr::Contr(Cam* cam, RangeF* range)
	: cam(cam), range(range), acceptor(ioService, ip::tcp::endpoint(ip::tcp::v4(), 11111)),
	clientSocket(ioService)
{
	acceptor.accept(clientSocket);
	cout << "Established connection with: " << clientSocket.remote_endpoint() << endl;
}


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

	cv::putText(cam->res, distance, cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX, 3.0, cv::Scalar(255, 255, 255), 4);
	//cv::imshow("RTSP stream + Thermal output", cam->res);

	/*bool success = cv::imwrite(cam->filename, cam->res);

	if (success)
	{
		std::cout << "Image saved as " << cam->filename << std::endl;
	}
	else
	{
		std::cout << "Error: Image not saved." << std::endl;
	}*/

	std::vector<uchar> encoded;
	cv::imencode(".jpg", cam->res, encoded);

	uint64_t msg_size = encoded.size();
	clientSocket.write_some(boost::asio::buffer(&msg_size, sizeof(msg_size)));
	clientSocket.write_some(boost::asio::buffer(encoded.data(), encoded.size()));

	cv::imshow("SOURCE", cam->res);
	char key = cv::waitKey(1);
	if (key == 'q') {
		clientSocket.close();
		std::cout << "Connection closed" << std::endl;
	}
		
	acceptor.close();

	//open = cv::imread(cam->filename);
	//cv::imshow("Image", open);
}
