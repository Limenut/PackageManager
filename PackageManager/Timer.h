#pragma once

#include <chrono>
#include <ratio>
#include <thread>

using namespace std::chrono;

class Timer
{
public:
	Timer();
	void start(int _cycleTime);
	void reset();
	void startCycle();
	bool finish();
	void rush();

	int cycleTime;
	bool running; //this is not currently used

private:
	steady_clock::time_point timeNow;
	steady_clock::time_point timeNext;
};