#pragma once
#include <string>
#include <boost/asio.hpp>
#include "SharedData.h"
using namespace boost::asio;
using ip::tcp;

class WeatherFetcher
{
	SharedData& shared_data;
	int const REFRESH_TIME = 1;
	std::string readChunkedResponse(tcp::socket& socket);
	void callAPI();

public:
	WeatherFetcher(SharedData& shared);
	void operator()();
};