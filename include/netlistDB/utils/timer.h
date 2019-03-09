#pragma once
#include <chrono>
#include <string>

namespace netlistDB {
namespace utils {

// timer object which prints the time with info message on delete
class Timer {
	const std::string prompt;
	typename std::chrono::system_clock::time_point start;
public:
	/*
	 * @param prompt the message which will be added to message with the time
	 * */
	Timer(const std::string & prompt);
	~Timer();
};

}
}
