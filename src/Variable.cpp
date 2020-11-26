#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Variable.hpp"

namespace var{
	std::vector<std::string> varidentifier;
	std::vector<std::string> varvalue;

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
	std::string search(std::string iden){
		auto it = std::find(varidentifier.begin(), varidentifier.end(), iden);
		if (it == varidentifier.end())	{
		  return "NULL";
		}else{
		  int index = std::distance(varidentifier.begin(), it);
		  return varvalue[index];
		}
	}

	unsigned long int count(void){
		return varidentifier.size();
	}

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

	std::vector<std::string> globals(void){
		return varidentifier;
	}


}


std::vector<std::string> subVec(std::vector<std::string> vec, unsigned long start, unsigned long end){
	std::vector<std::string> output;
	for(;start < end; start++){
		output.push_back(vec[start]);
	}
	return output;
}