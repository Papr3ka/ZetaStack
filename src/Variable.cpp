#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<thread>

#include "Variable.hpp"
#include "Entropy.hpp"

namespace var{

	// specials to be moved to separate file for more accurate processing
	std::vector<std::string> specialIden{
		"pi"
	};

	std::vector<std::string> varidentifier{
		"pi"
	};
	std::vector<std::string> varvalue{
		"3.14159265"
	};

	unsigned long int buffermax = 4096;
	bool runbuffer = true;

	long prevr = rand() % 769 + 47; // for random nums

	static std::vector<long int> randbuffer;

	static long getrand(void){
		long int retval;
		if(!randbuffer.empty()){
			retval = randbuffer.front();
			randbuffer.erase(randbuffer.begin());
			return retval%1000;
		}else{
			prevr = getrandnum(prevr) ^ 127; // Good luck trying to reverse engineer it
			return prevr%1000;
		}
	}

	unsigned long int getrandbufsize(void){
		return randbuffer.size();
	}

	void setbuffermax(unsigned long int setval){
		buffermax = setval;
		if(setval == 0){
			std::vector<long int>().swap(randbuffer);
		}
		while(randbuffer.size() > buffermax){
			randbuffer.erase(randbuffer.begin());
		}
		randbuffer.shrink_to_fit();
		return;
	}

	unsigned long int getbuffermax(void){
		return buffermax;
	}

	void clearbuffer(void){
		std::vector<long int>().swap(randbuffer);
		return;
	}

	void joinbuffer(void){
		runbuffer = false;
		return;
	}

	// Add variable to vector
	void update(std::string iden, std::string val){
		auto it = std::find(varidentifier.begin(), varidentifier.end(), iden);
		if (it == varidentifier.end()){
		  	varidentifier.push_back(iden);
			varvalue.push_back(val);
		}else{
		  int index = std::distance(varidentifier.begin(), it);
		  varvalue.erase(varvalue.begin()+index);
		  varvalue.insert(varvalue.begin()+index,1,val);
		}

	}

	// Return var value if identifier is found else return NULL
	std::string search(std::string iden){
		auto it = std::find(varidentifier.begin(), varidentifier.end(), iden);
		if (it == varidentifier.end()){
			if(iden == "rand"){
				return std::to_string(getrand());
			}else{
	  			return "NULL";
	  		}
		}else{
		  int index = std::distance(varidentifier.begin(), it);
		  return varvalue[index];
		}
	}

	// Wrapper for amount of variables
	unsigned long int count(void){
		return varidentifier.size();
	}

	// Delete variable, return 1 if not present 0 if success
	// 2 if cannot be deleted
	int delvar(std::string variden){
		for(std::string chkstr : specialIden){
			if(variden == chkstr) return 3;
		}
		auto it = std::find(varidentifier.begin(), varidentifier.end(), variden);
		if (it == varidentifier.end()){
			return 1;
		}else{
			int index = std::distance(varidentifier.begin(), it);
			varidentifier.erase(varidentifier.begin()+index);
			varvalue.erase(varvalue.begin()+index);
			return 0;
		}
	}

	// Wrapper for all names
	std::vector<std::string> globals(void){
		return varidentifier;
	}


	void buffer(bool run){
		if(!run){
			return;
		}
		while(runbuffer && run){
			if(randbuffer.size() < buffermax){
				while(randbuffer.size() < buffermax){
					prevr = getrandnum(prevr) ^ 127;
					randbuffer.push_back(prevr);
				}
			}else{
				std::this_thread::sleep_for(std::chrono::milliseconds(125));
			}
		}
		return;
	}
}

// Like substr but for vectors
std::vector<std::string> subVec(std::vector<std::string> vec, unsigned long start, unsigned long end){
	std::vector<std::string> output;
	for(;start < end; start++){
		output.push_back(vec[start]);
	}
	return output;
}