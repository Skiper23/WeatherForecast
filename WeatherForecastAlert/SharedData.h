#ifndef SHARED_DATA_H
#define SHARED_DATA_H
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

class SharedData
{
public:
	std::queue <std::string> thread_queue;
	std::mutex thread_mutex;
	std::condition_variable thread_condition_variable;
};
#endif 