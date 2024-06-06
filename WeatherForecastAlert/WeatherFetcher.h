#pragma once
#include <string>
#include <boost/asio.hpp>
using namespace boost::asio;
using ip::tcp;

class WeatherFetcher
{
	double latitude, longitude;
public:
	WeatherFetcher();
	std::string readChunkedResponse(tcp::socket& socket);
	void callAPI();

};