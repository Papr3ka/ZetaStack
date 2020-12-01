#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cstdlib>

#include "Status.hpp"

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

	short prec = 5;

	float barlen = 50;
	float countmax;
	float dispbar;
	float percent;

	unsigned long long int rcount = 0;

	void init(long int max){	
		rcount = 0;
		countmax = max;
		return;
	}

	void update(float count){
		count++;
		barbody = (loadchar*dispbar).append(spacechar*(barlen-dispbar));
		dispbar = barlen-(count/countmax)*barlen;
		percent = 100-count/countmax*100;
		std::cout << "\r|" << barbody << "| " << trunc(percent,prec) << "%\r";
		return;
	}

	void finish(void){
		std::cout << spacechar*(barlen*2+prec+5) << "\r";
		return;
	}


}