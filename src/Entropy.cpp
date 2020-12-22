#include<chrono>
#include<cstdlib>
#include<iostream>

#include "Entropy.hpp"
/*
The CIA encourages code obfuscation. They make it more complicated than necessary.
—Terry A. Davis
*/

long urandom(long seed){
	long wait = 1;
	long x=2;

	long tepoch = std::chrono::duration_cast<std::chrono::seconds>
						(std::chrono::system_clock::now().time_since_epoch()).count();

	std::chrono::_V2::steady_clock::time_point start;
	std::chrono::_V2::steady_clock::time_point end;

	start = std::chrono::steady_clock::now();

	for(long cnt = 0;
		cnt < (tepoch % 8191+65521) * seed %131071;
		cnt++){

		wait = x*x-x;

	}
	end = std::chrono::steady_clock::now();
	return (std::chrono::duration<long, std::nano>(end - start).count() % 
		   (tepoch % 8191+65521-seed)* seed %131071+wait) % 1021;
}

long int getrandnum(long seed){
	long int t = urandom(seed);
	t ^= t << 23;
	t ^= t >> 17;
	t ^= seed ^ (seed >> 26);
	return std::abs(urandom(t^seed)-t*seed);
}

bool getrandbit(long seed){
	return (getrandnum(seed) ^ seed) % 2;
}

long int getrandl(long seed){
	long long int output = 0;
	for(long long int bitpos = 0; bitpos < 32; bitpos++){
		if(getrandbit(seed)){
			output += 1 << bitpos;
		}
	}
	return (long int)(output+seed) ^ seed;
}

unsigned long long int getrandull(long seed){
	long long int output = 0;
	for(unsigned long long int bitpos = 0; bitpos < 64; bitpos++){
		if(getrandbit(seed)){
			output += 1 << bitpos;
		}
	}
	return output;
}

