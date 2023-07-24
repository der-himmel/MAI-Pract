//zoom (+focus)
//sudo g++ control<>.cpp -o <name> -lboost_system -lcrypto -lcpprest -pthread


#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <time.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

int main() {
    // IP address and port of the camera
    std::string cameraIpAddress = "172.16.0.5";
    std::string cameraPort = "80";
    
    std::string username = "admin";
    std::string password = "Admin123";
    
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
           
    http_client_config config;
    config.set_credentials(web::credentials(username, password));
    
    http_client client(zoomOutUri.to_uri(), config); //or http_client client(zoomInUri.to_uri(), config)
    
    while(true)
    {
		client.request(methods::GET).then([](http_response response) {
			std::cout << "Zoom out command sent. Response code: " << response.status_code() << std::endl;
		}).wait();
		sleep(1);
	}

    

    /*// Command for Focus (adjusting focus level)
    uri_builder focusUri;
    focusUri.set_scheme("http")
            .set_host(cameraIpAddress)
            .set_port(cameraPort)
            .set_path("/focus.cgi")
            .append_query("focus", "near");
    client = http_client(focusUri.to_uri());
    client.request(methods::GET).then([](http_response response) {
        std::cout << "Focus command sent. Response code: " << response.status_code() << std::endl;
    }).wait();*/

    return 0;
}

