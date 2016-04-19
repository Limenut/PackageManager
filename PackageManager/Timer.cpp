#include "Timer.h"

Timer::Timer()
{
	running = false;
}

void Timer::start(int _cycleTime)
{
	cycleTime = _cycleTime;
	startCycle();
	reset();
	running = true;
}

void Timer::startCycle()
{
	timeNow = steady_clock::now();
}

bool Timer::finish()
{
	//if (!running) return false;
	//running = false;
	return (timeNow >= timeNext);
}

void Timer::reset()
{
	timeNext = timeNow + milliseconds(cycleTime);
}

void Timer::rush()
{
	timeNow = timeNext;
}