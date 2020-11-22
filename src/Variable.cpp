#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Variable.hpp"

namespace var{
	std::vector<std::string> identifier;
	std::vector<std::string> value;
	void update(std::string iden, std::string val){
		auto it = std::find(identifier.begin(), identifier.end(), iden);
		if (it == identifier.end())	{
		  	identifier.push_back(iden);
			value.push_back(val);
		}else{
		  int index = std::distance(identifier.begin(), it);
		  value.erase(value.begin()+index);
		  value.insert(value.begin()+index,1,val);
		}

	}
	std::string search(std::string iden){
		auto it = std::find(identifier.begin(), identifier.end(), iden);
		if (it == identifier.end())	{
		  return "NULL";
		}else{
		  int index = std::distance(identifier.begin(), it);
		  return value[index];
		}
	}

	unsigned long int count(void){
		return identifier.size();
	}

	int delvar(std::string variden){
		auto it = std::find(identifier.begin(), identifier.end(), variden);
		if (it == identifier.end()){
			return 1;
		}else{
			int index = std::distance(identifier.begin(), it);
			identifier.erase(identifier.begin()+index);
			value.erase(value.begin()+index);
			return 0;
		}
	}

	std::vector<std::string> globals(void){
		return identifier;
	}

}


std::vector<std::string> subVec(std::vector<std::string> vec, unsigned long start, unsigned long end){
	std::vector<std::string> output;
	for(;start < end; start++){
		output.push_back(vec[start]);
	}
	return output;
}