#include<chrono>
#include<iostream>

#include "Entropy.hpp"

long urandom(long seed){
	volatile long wait = 1;
	volatile long x=2;

	register long tepoch = std::chrono::duration_cast<std::chrono::seconds>
						(std::chrono::system_clock::now().time_since_epoch()).count();

	std::chrono::_V2::steady_clock::time_point start;
	std::chrono::_V2::steady_clock::time_point end;

	start = std::chrono::steady_clock::now();

	for(volatile long cnt = 0;
		cnt < (tepoch % 8191+65521)* seed %131071;
		cnt++){

		wait = x*x-x;

	}
	end = std::chrono::steady_clock::now();
	return (std::chrono::duration<long, std::nano>(end - start).count() % 
		   (tepoch % 8191+65521-seed)* seed %131071+wait) % 1021;
}
