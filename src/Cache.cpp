#include<algorithm>
#include<iostream>
#include<string>
#include<vector>

#include "Cache.hpp"

namespace cch {

	unsigned long int maxlen = 1024; // How big the cache can be before discarding

	bool cache_enable = true;

	std::vector<std::string> identifier;
	std::vector<std::string> value;
	std::vector< std::vector <std::string> > metadata;

	std::vector<std::string> requestqueue;

	std::vector<std::string> reservedmeta{
		"ans",
		"rand"
	};

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
		std::vector< std::vector<std::string> >().swap(metadata);
		return;
	}

	void update(std::string iden, std::string val, std::vector<std::string> depends){
		unsigned long int erase_idx=0;
		for(std::string tcheck: reservedmeta){
			erase_idx=0;
			while(erase_idx < depends.size()){
				if(depends[erase_idx] == tcheck){
					return;
				}else{
					erase_idx++;
				}	
			}
		}
		if(cache_enable){
			auto it = std::find(identifier.begin(), identifier.end(), iden);
			if(it == identifier.end()){
				identifier.insert(identifier.begin(),iden);
				value.insert(value.begin(),val);
				metadata.insert(metadata.begin(), depends);
			}else if(identifier.size() < maxlen){
				long int index = std::distance(identifier.begin(), it);
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
		if(metadata.size() <= 0) return;
		unsigned long int index = 0;
		std::vector<std::string> tempvec;
		while(index < metadata.size()){
			tempvec = metadata.at(index);
			for(std::string fmeta: tempvec){
				if(fmeta == change){
					identifier.erase(identifier.begin()+index);
					value.erase(value.begin()+index);
					metadata.erase(metadata.begin()+index);
				}else{
					index++;
					if(index > metadata.size()) return;
				}
			}
			if(tempvec.size() <= 0) index++;
		}
		return;
	}

	void add_depend(std::string change){
		requestqueue.emplace_back(change);
		return;
	}

	void fulfill_depends(void){
		for(std::string toref: requestqueue){
			refreshDepends(toref);
		}
		std::vector<std::string>().swap(requestqueue);
		return;
	}

	std::string search(std::string iden){
		if(cache_enable){
			auto it = std::find(identifier.begin(), identifier.end(), iden);
			if(it == identifier.end()){
				return "NULL";
			}else{
				long int index = std::distance(identifier.begin(), it);
				return value[index];
			}
		}else{
			return "NULL";
		}
	}
}