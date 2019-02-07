#pragma once
#include <chrono>
#include <string>

class Timer {
	const std::string prompt;
	typename std::chrono::system_clock::time_point start;
public:
	Timer(const std::string & prompt);
	~Timer();
};
