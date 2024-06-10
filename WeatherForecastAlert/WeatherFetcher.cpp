#include "WeatherFetcher.h"
#include "WeatherProcessor.h"
#include <iostream>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
WeatherFetcher::WeatherFetcher(SharedData& shared) : shared_data(shared) 
{
}

void WeatherFetcher::operator() ()
{
    while (true)
    {
        callAPI();
        boost::this_thread::sleep_for(boost::chrono::minutes(10));
    }
}

void WeatherFetcher::callAPI()
{
    try 
    {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("api.open-meteo.com", "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET /v1/forecast?latitude=53.07&longitude=20.14&hourly=temperature_2m,rain,wind_speed_10m HTTP/1.1\r\nHost: api.open-meteo.com\r\nAccept: */*\r\nConnection: close\r\n\r\n";
        write(socket, request);

        std::string response = readChunkedResponse(socket);

        {
            std::lock_guard<std::mutex> lock(shared_data.thread_mutex);
            shared_data.thread_queue.push(response);
        }

        shared_data.thread_condition_variable.notify_one();
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
std::string WeatherFetcher:: readChunkedResponse(tcp::socket& socket) 
{
    boost::asio::streambuf response;
    boost::system::error_code error;
    std::ostringstream response_stream;
    std::string line;

    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) 
    {
        if (error && error != boost::asio::error::eof) 
        {
            throw boost::system::system_error(error);
        }

        std::istream response_streambuf(&response);
        response_stream << response_streambuf.rdbuf();
    }

    std::string full_response = response_stream.str();
    std::size_t body_start = full_response.find("\r\n\r\n");
    if (body_start != std::string::npos) 
    {
        body_start += 4;
    }
    else 
    {
        return "";
    }

    std::string body = full_response.substr(body_start);
    std::istringstream body_stream(body);
    std::ostringstream final_body;
    std::string chunk_size_str;
    std::size_t chunk_size;
    int isfirst = 0;

    while (std::getline(body_stream, chunk_size_str)) 
    {
        std::istringstream(chunk_size_str) >> std::hex >> chunk_size;
        if (chunk_size == 0) 
        {
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