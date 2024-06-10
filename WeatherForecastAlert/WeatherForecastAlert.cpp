#include "SharedData.h"
#include "WeatherProcessor.h"
#include "WeatherFetcher.h"
#include <boost/thread.hpp>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

int main() {
    SharedData shared_data;

    WeatherFetcher weatcher_fetcher(shared_data);
    WeatherProcessor weatcher_processor(shared_data);

    boost::thread fetcher_thread(boost::ref(weatcher_fetcher));
    boost::thread processor_thread(boost::ref(weatcher_processor));

    fetcher_thread.join();
    processor_thread.join();
    return 0;
}
