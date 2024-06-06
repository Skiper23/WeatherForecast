#include <string>
#include "WeatherProcessor.h"
#include <boost/json.hpp>
#include <iostream>


WeatherProcessor :: WeatherProcessor(std::string input)
{
	data = input;
}

void WeatherProcessor:: ParseJSON()
{
	boost::json::value parseJson = boost::json::parse(data);
	boost::json::object parseObject = parseJson.as_object();

	double value = parseObject["longitude"].as_double();
	boost::json::object hourly = parseObject["hourly"].as_object();
	times = hourly["time"].as_array();
	temperatures = hourly["temperature_2m"].as_array();
	if(hourly.if_contains("rain"))
		rains = hourly["rain"].as_array();
	makeNotification();
}
void WeatherProcessor::makeNotification()
{
	int size = times.size();

	for (int i = 0; i<size; i++)
	{
		std::string message;
		if (temperatures[i].as_double() > 30.0 )
		{
			message = "Uwaga wysoka temperatura ";
			message += temperatures[i].as_string().c_str();
		}
		if (temperatures[i].as_double() < -10.0)
		{
			message = "Uwaga nieska temperatura ";
			message += temperatures[i].as_string().c_str();
		}
		if (rains.size() > 0)
		{
			if (rains[i].as_double() > 0.5)
			{
				if (!message.empty())
				{
					message += " oraz ";
				}
				message += "duze opady deszczu ";
			}
		}
		if (!message.empty())
		{
			std::cout << message << " w dniu " << times[i].as_string() << '\n';
		}

	}
}
