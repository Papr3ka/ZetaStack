#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

#include "Cache.hpp"

namespace cch {

	unsigned long int maxlen = 1024; // How big the cache can be before discarding

	bool cache_enable = true;

	std::vector<std::string> identifier;
	std::vector<std::string> value;
	std::vector< std::vector <std::string> > metadata;

	std::vector<std::string> getiden(void){
		return identifier;
	}

	std::vector<std::string> getval(void){
		return value;
	}

	void setstate(bool state){
		cache_enable = state;
		return;
	}

	bool getstate(void){
		return cache_enable;
	}

	void setmaxlen(unsigned long int ml){
		maxlen = ml;
		while(identifier.size() > maxlen){
			identifier.pop_back();
			value.pop_back();
			metadata.pop_back();
		}
		identifier.shrink_to_fit();
		value.shrink_to_fit();
		metadata.shrink_to_fit();
		return;
	}

	unsigned long int getmaxlen(void){
		return maxlen;
	}

	void reset(void){
		std::vector<std::string>().swap(identifier);
		std::vector<std::string>().swap(value);
		return;
	}

	void update(std::string iden, std::string val, std::vector<std::string> depends){
		if(cache_enable){
			auto it = std::find(identifier.begin(), identifier.end(), iden);
			if(it == identifier.end()){
				identifier.insert(identifier.begin(),iden);
				value.insert(value.begin(),val);
				metadata.insert(metadata.begin(), depends);
			}else if(identifier.size() < maxlen){
				int index = std::distance(identifier.begin(), it);
				identifier.erase(identifier.begin()+index);
				value.erase(value.begin()+index);
				identifier.insert(identifier.begin(),iden);
				value.insert(value.begin(),val);
				metadata.erase(metadata.begin()+index);
				metadata.insert(metadata.begin(), depends);
			}
		}
		return;
	}

	void refreshDepends(std::string change){
		unsigned long int index = 0;
		std::vector<std::string> tempvec;
		while(index < metadata.size()){
			tempvec = metadata.at(index);
			for(std::string fmeta: tempvec){
				if(fmeta == change){
					identifier.erase(identifier.begin()+index);
					value.erase(value.begin()+index);
					metadata.erase(metadata.begin()+index);
					index--;
				}	
			}
			index++;
		}
		return;
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