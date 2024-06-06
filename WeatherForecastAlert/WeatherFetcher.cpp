#include "WeatherFetcher.h"
#include "WeatherProcessor.h"
#include <iostream>
WeatherFetcher::WeatherFetcher()
{
    callAPI();
}

void WeatherFetcher::callAPI()
{
    try {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("api.open-meteo.com", "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        std::cout << "DU{PA";
        request_stream << "GET /v1/forecast?latitude=52.52&longitude=17.03&hourly=temperature_2m,rain HTTP/1.1\r\nHost: api.open-meteo.com\r\nAccept: */*\r\nConnection: close\r\n\r\n";
        write(socket, request);

        std::string response = readChunkedResponse(socket);
        std::cout << response << std::endl;
        WeatherProcessor weatherProcessor(response);

        weatherProcessor.ParseJSON();


    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
std::string WeatherFetcher:: readChunkedResponse(tcp::socket& socket) {
    boost::asio::streambuf response;
    boost::system::error_code error;
    std::ostringstream response_stream;
    std::string line;

    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
        if (error && error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        std::istream response_streambuf(&response);
        response_stream << response_streambuf.rdbuf();
    }

    std::string full_response = response_stream.str();
    std::size_t body_start = full_response.find("\r\n\r\n");
    if (body_start != std::string::npos) {
        body_start += 4;
    }
    else {
        return "";
    }

    std::string body = full_response.substr(body_start);
    std::istringstream body_stream(body);
    std::ostringstream final_body;
    std::string chunk_size_str;
    std::size_t chunk_size;
    int isfirst = 0;


    while (std::getline(body_stream, chunk_size_str)) {
        std::istringstream(chunk_size_str) >> std::hex >> chunk_size;
        if (chunk_size == 0) {
            break;
        }

        std::vector<char> buffer(chunk_size);
        body_stream.read(buffer.data(), chunk_size);
        final_body.write(buffer.data(), chunk_size);

        // Ignore the trailing \r\n after the chunk
        body_stream.ignore(2);
    }


    return final_body.str();
}