#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

int main() {
    // IP address and port of the camera
    std::string cameraIpAddress = "192.168.1.100";
    std::string cameraPort = "80";

    // Command for Zoom (adjusting zoom level)
    uri_builder zoomUri;
    zoomUri.set_scheme("http")
           .set_host(cameraIpAddress)
           .set_port(cameraPort)
           .set_path("/zoom.cgi")
           .append_query("zoom", "in");
    http_client client(zoomUri.to_uri());
    client.request(methods::GET).then([](http_response response) {
        std::cout << "Zoom command sent. Response code: " << response.status_code() << std::endl;
    }).wait();

    // Command for Focus (adjusting focus level)
    uri_builder focusUri;
    focusUri.set_scheme("http")
            .set_host(cameraIpAddress)
            .set_port(cameraPort)
            .set_path("/focus.cgi")
            .append_query("focus", "near");
    client = http_client(focusUri.to_uri());
    client.request(methods::GET).then([](http_response response) {
        std::cout << "Focus command sent. Response code: " << response.status_code() << std::endl;
    }).wait();

    return 0;
}
