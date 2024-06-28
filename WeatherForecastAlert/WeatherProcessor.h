#pragma once
#include <string>
#include <vector>
#include <boost/json.hpp>
#include "SharedData.h"

class WeatherProcessor
{
	SharedData& shared_data;
	std::string data;
	boost::json::array times;
	boost::json::array temperatures;
	boost::json::array rains;
	boost::json::array winds;
	double const UPPER_TEMPERATURE_ALERT = 10.0;
	double const LOWER_TEMPERATURE_ALERT = 0.0;
	double const RAIN_ALERT = 2.0;
	double const WIND_ALERT = 10.0;

	void ParseJSON(std::string data);
	void makeNotification();

public:
	WeatherProcessor(SharedData& shared);
	void operator()();

};