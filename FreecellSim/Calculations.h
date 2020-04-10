#pragma once
#include <chrono>
#include <iostream>
#include <string>

class Timer {
public:
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;
	std::string func;

	Timer() {
		start = std::chrono::high_resolution_clock::now();
		func = "This function";
	}

	Timer(const char functionName[]) {
		start = std::chrono::high_resolution_clock::now();
		func = functionName;
	}

	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		std::cout << func << " took " << duration.count() << "s " << std::endl;
	}

	void getCurrentDuration() {
		std::chrono::time_point<std::chrono::steady_clock> timeStamp = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> currentDuration = timeStamp - start;
		std::cout << func << " is already running for " << currentDuration.count() << "s " << std::endl;
	}
};