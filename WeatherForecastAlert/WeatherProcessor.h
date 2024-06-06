#pragma once
#include <string>
#include <vector>
#include <boost/json.hpp>

class WeatherProcessor
{
	std::string data;
	double latitude;
	double longitude;
	boost::json::array times;
	boost::json::array temperatures;
	boost::json::array rains;
public:
	WeatherProcessor(std::string input);
	void ParseJSON();
	void makeNotification();
};