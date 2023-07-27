std::vector<char> data(512);
	size_t len = clientSocket.read_some(buffer(data));

	if (len > 0) 
	{
		string received_data(data.begin(), data.begin() + len);
		//cout << "Received: " << received_data << endl;
		if (received_data == "zoomin")
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
		}
	}

	data.clear();
