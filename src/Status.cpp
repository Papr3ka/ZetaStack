#include<iostream>
#include<ostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<cmath>

#include<thread>
#include<atomic>

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
	std::string loadchar = "≡"; // ≡, =, -, █
	std::string spacechar = " ";

	// ◀ ▶ < >
	std::string right = "▶";
	std::string left = "◀";


	bool dostat = false;
	short prec = 5;

	float barlen = 50;
	float dotbarlen = barlen+17;

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
		countmax = max;
		count = 0;
		run_bar = true;
		return;
	}

	void start(void){
		start_time = std::chrono::steady_clock::now();
		spos = 0;
		dostat = true;
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

	// Actually update the bar
	void updatepercent(void){
		dispbar = barlen-(count/countmax)*barlen;
		barbody = (loadchar*dispbar).append(spacechar*(barlen-dispbar));
		percent = 100-count/countmax*100;
		elapsed = std::round(std::chrono::duration<double, std::milli>(next_time - start_time).count()/100)/10;
		std::cout << "\r|" << barbody << "| " << prec_str(elapsed) << "s | " << countmax - count << "/"<< countmax << " ["<< strunc(percent,prec) << "%] " << inform_mode <<"\r";
		std::cout.flush();
		return;
	}

	void updatecycle(void){
		if(direction){
			barbody = (spacechar*(float)spos).append(right).append(spacechar*(float)(dotbarlen-(spos+right.size())));
			elapsed = std::round(std::chrono::duration<double, std::milli>(next_time - start_time).count()/100)/10;
			std::cout << "\r|" << barbody << "| " << prec_str(elapsed) << "s | "<< inform_mode <<"   \r";
			std::cout.flush();
			spos++;
		}else{
			barbody = (spacechar*(float)spos).append(left).append(spacechar*(float)(dotbarlen-(spos+left.size())));
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
		loadtype = 0;
		inform_mode = "";
		return;
	}

	// This is the function that the thread runs
	void barmanager(void){
		right = right*8.0f;
		left = left*8.0f; 
		while(run_bar){
			if(dostat && state){
				next_time = std::chrono::steady_clock::now();
				switch(loadtype){
					case 0:
						updatecycle();
						std::this_thread::sleep_for(std::chrono::milliseconds(16));
						break;
					case 1:
						updatepercent();
						std::this_thread::sleep_for(std::chrono::milliseconds(25));
						break;
					default:
						std::this_thread::sleep_for(std::chrono::milliseconds(125));
						break;
				}
			}else{
				std::this_thread::sleep_for(std::chrono::milliseconds(125));
			}
		}
		return;
	}


}