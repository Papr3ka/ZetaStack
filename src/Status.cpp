#include<algorithm>
#include<atomic>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<ostream>
#include<string>
#include<thread>
#include<vector>

#include "Status.hpp"

// String multiplication
static std::string operator * (std::string str, float mul){
	if(mul < 0){
		return str;
	}
	std::string output;
	for(float count = mul;count > 0; count--){
		output += str;
	}
	return output;
}

static std::string operator * (std::string str, long int mul){
	if(mul < 0){
		return str;
	}
	std::string output;
	for(long int count = mul;count > 0; count--){
		output += str;
	}
	return output;
}

std::string strunc(float x, short prec){
	if(std::isnan(x)) return "0";
	std::string out = std::to_string(x);
	if((short)out.size() <= prec){
		return out.substr(0,out.size());
	}else{
		return out.substr(0, prec);
	}
}

std::string strunc(double x, unsigned long  int prec){
	if(std::isnan(x)) return "0";
	std::string out = std::to_string(x);
	if(out.size() <= prec){
		return out.substr(0,out.size());
	}else{
		return out.substr(0, prec);
	}
}

std::string prec_str(double fstr){
	std::string str = std::to_string(fstr);
	str = str.substr(0,str.size()-5);
	if(str.find(".") == std::string::npos){
		str.append(".0");
	}
	return str;

}

namespace bar {

	std::chrono::_V2::steady_clock::time_point start_time;
	std::chrono::_V2::steady_clock::time_point next_time;
	double elapsed;

	std::string barbody;
	std::string inform_mode = "";

	#ifndef NO_UNICODE_STATUS_
		
		std::string loadchar = "≡"; // ≡, =, -, █ |
		std::string spacechar = " ";

		// ◀ ▶ < >
		std::string right = "▶";
		std::string left = "◀";
	#else
		
		std::string loadchar = "="; 
		std::string spacechar = " ";

		std::string right = ">";
		std::string left = "<";		
	#endif


	bool dostat = false;
	constexpr float prec = 5;

	constexpr float barlen = 50;
	constexpr float dotbarlen = barlen+17;

	std::atomic<float> countmax(0);
	std::atomic<float> dispbar(0);
	std::atomic<float> percent(0);

	std::atomic<float> count(1);
	std::atomic<bool> run_bar(true);
	std::atomic<int> loadtype(0);
	/*
		0 - dot load
		1 - bar/ percent
		2 - WIP
		3 - WIP
	*/

	unsigned long long int rcount = 0;

	long int spos = 0; 
	bool direction = true; // true right, false left

	bool state;

	void setstate(bool mode){
		state = mode;
		return;
	}

	void init(long int max){
		dostat = true;	
		rcount = 0;
		countmax = (float)max;
		count = 0;
		run_bar = true;
		return;
	}

	void start(void){
		start_time = std::chrono::steady_clock::now();
		spos = 0;
		dostat = true;
		state = true;
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

	void inform(std::string md){
		inform_mode = md;
		std::cout << spacechar*(barlen*2+prec+5) << "    \r";
		return;
	}

	void changemode(int m){
		loadtype = m;
		return;
	}

	// Use via Executer to update progress count float
	void setcount(float x){
		count = x;
		return;
	}

	// Overload
	void setcount(unsigned long int x){
		count = (float)x;
		return;
	}

	// Actually update the bar
	inline void updatepercent(void){
		dispbar = (count/countmax)*barlen;
		barbody = (loadchar*dispbar).append(spacechar*(barlen-dispbar));
		percent = count/countmax*100;
		elapsed = std::round(std::chrono::duration<double, std::milli>(next_time - start_time).count()/100)/10;
		std::cout << "\r|" << barbody << "| " << prec_str(elapsed) << "s | " << count << "/"<< countmax << " ["<< strunc(percent,prec) << "%] " << inform_mode <<"\r";
		std::cout.flush();
		return;
	}

	inline void updatecycle(void){
		if(direction){
			barbody = (spacechar*spos).append(right).append(spacechar*(dotbarlen-(spos+right.size())));
			elapsed = std::round(std::chrono::duration<double, std::milli>(next_time - start_time).count()/100)/10;
			std::cout << "\r|" << barbody << "| " << prec_str(elapsed) << "s | "<< inform_mode <<"   \r";
			std::cout.flush();
			spos++;
		}else{
			barbody = (spacechar*spos).append(left).append(spacechar*(dotbarlen-(spos+left.size())));
			elapsed = std::round(std::chrono::duration<double, std::milli>(next_time - start_time).count()/100)/10;
			std::cout << "\r|" << barbody << "| " << prec_str(elapsed) << "s | "<< inform_mode << "   \r";
			std::cout.flush();
			spos--;
		}
		if((spos + right.size()) >= dotbarlen){
			direction = false;
		}else if(spos <= 0){
			direction = true;
		}
		return;
	}

	void finish(void){
		std::cout << spacechar*(barlen*2+prec+5) << "\r";
		std::cout.flush();
		dostat = false;
		state = false;
		loadtype = 0;
		inform_mode = "";
		return;
	}

	// This is the function that the thread runs
	void barmanager(void){
		right = right*8.0f;
		left = left*8.0f; 
		while(run_bar){
			backtoloop:
			if(dostat && state){
				next_time = std::chrono::steady_clock::now();
				switch(loadtype){
					case 0:
						updatecycle();
						if(!state) goto backtoloop;
						std::this_thread::sleep_for(std::chrono::milliseconds(16));
						if(!state) goto backtoloop;
						break;
					case 1:
						updatepercent();
						if(!state) goto backtoloop;
						std::this_thread::sleep_for(std::chrono::milliseconds(25));
						if(!state) goto backtoloop;
						break;
					default:
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
						break;
				}
			}else{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
		return;
	}


}