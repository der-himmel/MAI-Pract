#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include <thread>
#include <future>
#include <time.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include "Contr.h"
#include "RangeF.h"
#include "Cam.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std;
using namespace boost::asio;

Contr::Contr(Cam* cam, RangeF* range)
	: cam(cam), range(range), acceptor(ioService, ip::tcp::endpoint(ip::tcp::v4(), 11111)),
	clientSocket(ioService)
{
	acceptor.accept(clientSocket);
	cout << "Established connection with: " << clientSocket.remote_endpoint() << endl;

	uri_builder zoomOutUri;
	zoomOutUri.set_scheme("http")
		.set_host(cameraIpAddress)
		.set_port(cameraPort)
		.set_path("/ptzctrl.cgi?-step=0&-act=zoomout&-speed=1")
		.append_query("zoom", "out");

	uri_builder zoomInUri;
	zoomInUri.set_scheme("http")
		.set_host(cameraIpAddress)
		.set_port(cameraPort)
		.set_path("/ptzctrl.cgi?-step=0&-act=zoomin&-speed=1")
		.append_query("zoom", "in");

	uri_builder focusOutUri;
	focusOutUri.set_scheme("http")
		.set_host(cameraIpAddress)
		.set_port(cameraPort)
		.set_path("/ptzctrl.cgi?-step=0&-act=focusout&-speed=1")
		.append_query("focus", "out");

	uri_builder focusInUri;
	focusInUri.set_scheme("http")
		.set_host(cameraIpAddress)
		.set_port(cameraPort)
		.set_path("/ptzctrl.cgi?-step=0&-act=focusin&-speed=1")
		.append_query("focus", "in");

	http_client_config config;
	config.set_credentials(web::credentials(username, password));

	http_client zout(zoomOutUri.to_uri(), config);
	http_client zin(zoomInUri.to_uri(), config);
	http_client fout(focusOutUri.to_uri(), config);
	http_client fin(focusInUri.to_uri(), config);
}


void Contr::init()
{
	while (1)
	{
		operating();
	}
}

void Contr::recording()
{
	cam->update();

	cv::putText(cam->res, std::to_string(range->dist), cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX, 3.0, cv::Scalar(255, 255, 255), 4);
	
	cv::imshow("SOURCE", cam->res);
	//cv::waitKey(1);

	std::vector<uchar> encoded;
	cv::imencode(".png", cam->res, encoded);

	uint64_t msg_size = encoded.size();
	clientSocket.write_some(boost::asio::buffer(&msg_size, sizeof(msg_size)));
	clientSocket.write_some(boost::asio::buffer(encoded.data(), encoded.size()));
	
	char key = cv::waitKey(2);
	if (key == 'q') {
		clientSocket.close();
		std::cout << "Connection closed" << std::endl;
	}

	//acceptor.close();
}

void Contr::listening()
{
	range->getPacket();
	
	std::vector<char> data(512);
	size_t len = clientSocket.read_some(buffer(data));

	if (len > 0) 
	{
		string received_data(data.begin(), data.begin() + len);
		//cout << "Received: " << received_data << endl;
		/*if (received_data == "zoomin")
		{
			std::cout << "Zoom in command sent" << std::endl;

			zin.request(methods::GET).then([](http_response response) 
				{
				std::cout << "Zoom in command sent. Response code: " << response.status_code() << std::endl;
				}).wait();
		}
		else if (received_data == "zoomout")
		{
			std::cout << "Zoom out command sent" << std::endl;

			zout.request(methods::GET).then([](http_response response)
				{
					std::cout << "Zoom out command sent. Response code: " << response.status_code() << std::endl;
				}).wait();
		}
		else if (received_data == "focusin")
		{
			std::cout << "Focus in command sent" << std::endl;

			fin.request(methods::GET).then([](http_response response)
				{
					std::cout << "Focus in command sent. Response code: " << response.status_code() << std::endl;
				}).wait();
		}
		else if (received_data == "focusout")
		{
			std::cout << "Focus out command sent" << std::endl;

			fout.request(methods::GET).then([](http_response response)
				{
					std::cout << "Focus out command sent. Response code: " << response.status_code() << std::endl;
				}).wait();
		}*/
	}

	data.clear();
	sleep(0.25);
}

void Contr::operating()
{
	auto cams = std::async(&Contr::recording, this);
	auto comm = std::async(&Contr::listening, this);
	
	//if(cams.joinable()){cams.detach();}
	//if(comm.joinable()){comm.detach();}
	
	//cams.join();
	//comm.join();
}
