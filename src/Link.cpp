#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>

#include "Link.hpp"
#include "Function.hpp"

long long int recursedepth = 16; // Default 4096

namespace ld {

	static bool isEqual(std::vector<std::string> const &fcompvec, std::vector<std::string> const &bcompvec)
	{
	    auto pair = std::mismatch(fcompvec.begin(), fcompvec.end(), bcompvec.begin());
	    return pair.first == fcompvec.end() && pair.second == bcompvec.end();
	}
 

	// Do one round of function linking
	std::vector<std::string> link(std::vector<std::string> tokens){
		std::string funcname;
		std::vector<std::string> output;
		std::vector<std::string> temp;
		std::vector<std::string> temppack;
		std::vector< std::vector<std::string> > argpackage;
		long int braccounter;
		long int tpos = 0;
		while(!tokens.empty()){
			if(tokens.front().back() == '('){
				braccounter = 1;
				funcname = tokens.front();
				tokens.erase(tokens.begin());
				tokens.erase(tokens.begin());
				output.push_back("L_BRAC");
				while(braccounter != 0 && !tokens.empty()){
					std::cout << tokens.front() << ", ";
					if(tokens.front() == "L_BRAC"){
						braccounter++;
						temppack.push_back("L_BRAC");
						tokens.erase(tokens.begin());
						tpos--;
					}else if(tokens.front() == "R_BRAC"){
						braccounter--;
						temppack.push_back("R_BRAC");
						tokens.erase(tokens.begin());
						tpos--;
					}else{
						output.push_back(tokens.front());
						tokens.erase(tokens.begin());	
						tpos--;					
					}
					if(tokens.empty()) break;
					if(tokens.front() == "SEP" || tokens.empty()){
						tokens.erase(tokens.begin());
						argpackage.push_back(temppack);
						temppack.clear();
						tpos--;
					}	
				}
				temp = call(argpackage, funcname);
				output.insert(output.begin() + tpos,temp.begin(),temp.end());
				temp.clear();
				clearfuncdata(funcname);
			}else{
				output.push_back(tokens.front());
				tokens.erase(tokens.begin());
			}	
		}
		return output;
	}

	std::vector<std::string> recurselink(std::vector<std::string> tokens, long long int recursecount){
		std::vector<std::string> prev = link(tokens);
		if(!(isEqual(prev, tokens) ^ (recursecount >= recursedepth))){
			return recurselink(prev, recursecount);
			recursecount++;
		}
		return tokens;
		
		
	}
}
