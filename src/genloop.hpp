/*******************************************************************************
Generic loop.
*******************************************************************************/
#pragma once
#include <chrono>	// Time.
#include <thread>	// Sleep.
#ifdef DEBUG_LOOP
#include <iostream>	// Debug info.
#define DOUT(x) std::cout<<x
#else
#define DOUT(x)
#endif // DEBUG_LOOP
/*
period: Update period in milliseconds.
updater: bool() Function called every period.
renderer: void(float)
*/
template <int period=16,class T_update,class T_render>
void Loopy(T_update& updater,T_render& renderer)
{
	static_assert(period>0);
	using namespace std::chrono_literals;
	// We use microseconds to minimize time bleed in accumulator.
	typedef std::chrono::high_resolution_clock Chrono;
	typedef std::chrono::high_resolution_clock::time_point Time;
	typedef std::chrono::microseconds Duration;
	const std::chrono::milliseconds periodms(period);
	const Duration ticksPerUpdate(periodms);
	const Duration hardCap=4000ms;
	Time ticksNow;
	Time ticksPrev=Chrono::now();
	Duration acc(0);
	Duration delta(0);
#ifdef DEBUG_LOOP
	Duration debugCounter(0);
	Time debugStart=Chrono::now();
#endif
	bool ok=true;
	while (ok)
	{
		DOUT("[");
		ticksNow=Chrono::now();
		delta=std::chrono::duration_cast<Duration>(ticksNow-ticksPrev);
		DOUT("Delta="<<delta.count()<<" ");
		if (delta>hardCap)
		{
			delta=hardCap;
		}
		acc+=delta;
#ifdef DEBUG_LOOP
		debugCounter+=delta;
#endif
		ticksPrev=ticksNow;
		DOUT("Acc="<<acc.count()<<" ");
		if (acc>=ticksPerUpdate)
		{
			while (acc>=ticksPerUpdate)
			{
				DOUT("U.");
				ok&=updater();
				acc-=ticksPerUpdate;
			}
		}
		else
		{
			DOUT("Sleep.");
			std::this_thread::sleep_for(1ms);
		}
		float p=static_cast<float>(acc.count())/static_cast<float>(ticksPerUpdate.count());
		DOUT("R("<<p<<").");
		renderer(p);
		std::this_thread::sleep_for(1ms);
		DOUT("]\n");
	}
	DOUT("debugCounter="<<debugCounter.count()<<'\n');
	DOUT("Total time="<<std::chrono::duration_cast<Duration>(Chrono::now()-debugStart).count()<<'\n');
}