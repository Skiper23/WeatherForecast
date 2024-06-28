#include <string>
#include "WeatherProcessor.h"
#include <boost/json.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>


WeatherProcessor::WeatherProcessor(SharedData& shared) : shared_data(shared)
{
}

void WeatherProcessor::operator() ()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(shared_data.thread_mutex);
		shared_data.thread_condition_variable.wait(lock, [this]() 
		{
			return !shared_data.thread_queue.empty();
		});
		std::string data = shared_data.thread_queue.front();
		shared_data.thread_queue.pop();
		lock.unlock();
		ParseJSON(data);
	}
}

void WeatherProcessor:: ParseJSON(std::string data)
{
	try
	{
		boost::json::value parseJson = boost::json::parse(data);
		boost::json::object parseObject = parseJson.as_object();

		double value = parseObject["longitude"].as_double();
		boost::json::object hourly = parseObject["hourly"].as_object();
		times = hourly["time"].as_array();
		temperatures = hourly["temperature_2m"].as_array();
		if (hourly.if_contains("rain"))
		{
			rains = hourly["rain"].as_array();
		}
		if (hourly.if_contains("wind_speed_10m"))
		{
			winds = hourly["wind_speed_10m"].as_array();
		}

		makeNotification();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}
void WeatherProcessor::makeNotification()
{
	int size = times.size();
	boost::asio::io_service io_service;
	boost::asio::deadline_timer timer(io_service);
	timer.expires_at(boost::posix_time::second_clock::universal_time());
	std::cout << "Aktualizacja o " << timer.expires_at()<< " UTC \n";
	for (int i = 0; i < size; i++)
	{
		std::string message = "";
		if (temperatures[i].as_double() > UPPER_TEMPERATURE_ALERT)
		{
			message = "UWAGA wysoka temperatura ";
			message += std::to_string(temperatures[i].as_double());
		}
		if (temperatures[i].as_double() < LOWER_TEMPERATURE_ALERT)
		{
			message = "UWAGA niska temperatura ";
			message += std::to_string(temperatures[i].as_double());
		}
		if (rains.size() > 0)
		{
			if (rains[i].as_double() > RAIN_ALERT)
			{
				if (!message.empty())
				{
					message += " oraz ";
				}
				message += "duze opady deszczu ";
				message += std::to_string(rains[i].as_double());
			}
		}
		if (winds.size() > 0)
		{
			if (winds[i].as_double() > WIND_ALERT)
			{
				if (!message.empty())
				{
					message += " oraz ";
				}
				message += "mocne podmuchy wiatru ";
				message += std::to_string(winds[i].as_double());
			}
		}
		if (!message.empty())
		{
			std::cout << message << " przewidywane " << times[i].as_string().c_str() << '\n';
		}
	}
	std::cout << "\n\n";
}
