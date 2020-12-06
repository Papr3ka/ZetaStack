#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cstdlib>

#include<thread>
#include<atomic>

#include "Status.hpp"

// String multiplication
static std::string operator * (std::string str, float mul){
	std::string output;
	for(float count = mul;count > 0; count--){
		output += str;
	}
	return output;
}

static std::string trunc(float x, short prec){
	std::string out = std::to_string(x);
	if((short)out.size() <= prec){
		return out.substr(0,out.size());
	}else{
		return out.substr(0, prec);
	}
}

namespace bar {
	std::string barbody;
	std::string loadchar = "=";
	std::string spacechar = " ";


	bool dostat = false;
	short prec = 5;

	float barlen = 50;
	std::atomic<float> countmax(0);
	std::atomic<float> dispbar(0);
	std::atomic<float> percent(0);

	std::atomic<float> count(1);
	std::atomic<bool> run_bar(true);
	//std::atomic<int> loadtype(0);
	/*
		0 - dot load
		1 - bar/ percent
		2 - WIP
		3 - WIP
	*/


	unsigned long long int rcount = 0;

	void init(long int max){
		dostat = true;	
		rcount = 0;
		countmax = max;
		count = 0;
		run_bar = true;
		return;
	}

	void stop(void){
		dostat = false;
		return;
	}

	void join(void){
		run_bar = false;
		return;
	}

	// Use via Executer to update progress count float
	void setcount(float x){
		count = x;
		return;
	}

	// Actually update the bar
	void update(void){
		dispbar = barlen-(count/countmax)*barlen;
		barbody = (loadchar*dispbar).append(spacechar*(barlen-dispbar));
		percent = 100-count/countmax*100;
		std::cout << "\r|" << barbody << "| " << trunc(percent,prec) << "%\r";
		std::cout.flush();
		return;
	}

	void finish(void){
		std::cout << spacechar*(barlen*2+prec+5) << "\r";
		std::cout.flush();
		dostat = false;
		return;
	}

	// This is the function that the thread runs
	void barmanager(void){
		while(run_bar){
			if(dostat){
				update();
				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}else{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
		return;
	}


}