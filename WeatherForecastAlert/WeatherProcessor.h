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

	void ParseJSON(std::string data);
	void makeNotification();

public:
	WeatherProcessor(SharedData& shared);
	void operator()();

};