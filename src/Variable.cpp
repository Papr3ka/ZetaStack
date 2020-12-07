#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Variable.hpp"
#include "Entropy.hpp"

namespace var{

	// specials to be moved to separate file for more accurate processing
	std::vector<std::string> specialIden{
		"pi",
	};
	std::vector<std::string> specialVal{
		"3.1415"

	};

	std::vector<std::string> varidentifier;
	std::vector<std::string> varvalue;

	long prevr = rand() % 769 + 47; // for random nums

	// Add variable to vector
	void update(std::string iden, std::string val){
		auto it = std::find(varidentifier.begin(), varidentifier.end(), iden);
		if (it == varidentifier.end())	{
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
		if (it == varidentifier.end())	{
			auto it2 = std::find(specialIden.begin(), specialIden.end(), iden);
			if(it2 == specialIden.end()){
				if(iden == "rand"){
					prevr = getrand(prevr) ^ 127; // Good luck trying to reverse engineer it
					return std::to_string(prevr%1000);
				}else{
		  			return "NULL";
		  		}
			}else{
		  		int index = std::distance(varidentifier.begin(), it);
		  		return specialVal[index];
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
	int delvar(std::string variden){
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


}

// Like substr but for vectors
std::vector<std::string> subVec(std::vector<std::string> vec, unsigned long start, unsigned long end){
	std::vector<std::string> output;
	for(;start < end; start++){
		output.push_back(vec[start]);
	}
	return output;
}