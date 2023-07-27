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


/*uri_builder zoomOutUri;
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
	http_client fin(focusInUri.to_uri(), config);*/
