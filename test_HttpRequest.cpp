#include "pch.h"
#include "HTTPRequest.hpp"
#include <iostream>

int main() {
    try
    {
        // you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
        http::Request request("http://test.com/test");

        // send a get request
        const http::Response getResponse = request.send("GET");
        std::cout << std::string(getResponse.body.begin(), getResponse.body.end()) << '\n'; // print the result

        // send a post request
        const http::Response postResponse = request.send("POST", "foo=1&bar=baz", {
            "Content-Type: application/x-www-form-urlencoded"
            });
        std::cout << std::string(postResponse.body.begin(), postResponse.body.end()) << '\n'; // print the result

        // pass parameters as a map
        //std::map<std::string, std::string> parameters = { {"foo", "1"}, {"bar", "baz"} };
        //const http::Response response = request.send("POST", parameters, {
        //    "Content-Type: application/x-www-form-urlencoded"
        //    });
        //std::cout << std::string(response.body.begin(), response.body.end()) << '\n'; // print the result
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
}
