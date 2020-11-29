#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

#include "Cache.hpp"

namespace cch {

	unsigned long int maxlen = 64; // How big the cache can be before discarding

	bool cache_enable = true;

	std::vector<std::string> identifier;
	std::vector<std::string> value;

	std::vector<std::string> getiden(void){
		return identifier;
	}

	std::vector<std::string> getval(void){
		return value;
	}

	void setstate(bool state){
		cache_enable = state;
	}

	bool getstate(void){
		return cache_enable;
	}

	void setmaxlen(unsigned long int ml){
		maxlen = ml;
	}

	unsigned long int getmaxlen(void){
		return maxlen;
	}

	void reset(void){
		identifier.clear();
		value.clear();
	}

	void update(std::string iden, std::string val){
		if(cache_enable){
			auto it = std::find(identifier.begin(), identifier.end(), iden);
			if(it == identifier.end()){
				identifier.insert(identifier.begin(),iden);
				value.insert(value.begin(),val);
			}else{
				int index = std::distance(identifier.begin(), it);
				identifier.erase(identifier.begin()+index);
				value.erase(value.begin()+index);
				identifier.insert(identifier.begin(),iden);
				value.insert(value.begin(),val);
			}
			if(identifier.size() > maxlen){
				identifier.pop_back();
				value.pop_back();
			}
		}
	}


	std::string search(std::string iden){
		if(cache_enable){
			auto it = std::find(identifier.begin(), identifier.end(), iden);
			if(it == identifier.end()){
				return "NULL";
			}else{
				int index = std::distance(identifier.begin(), it);
				return value[index];
			}
		}else{
			return "NULL";
		}
	}
}