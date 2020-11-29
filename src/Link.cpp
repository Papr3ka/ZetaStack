#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>

#include "Link.hpp"
#include "Function.hpp"

//#include "Debug.h"

long long int recursedepth = 16; // Default 4096

namespace ld {

	static bool isEqual(std::vector<std::string> const &fcompvec, std::vector<std::string> const &bcompvec)
	{
	    auto pair = std::mismatch(fcompvec.begin(), fcompvec.end(), bcompvec.begin());
	    return pair.first == fcompvec.end() && pair.second == bcompvec.end();
	}
 

	// Do one round of function linking
	std::vector<std::string> link(std::vector<std::string> tokens){
		//dbg::printvec<std::string>(tokens);
		//std::cout << " input\n";
		std::vector<std::string> output;
		std::vector<std::string> temp;
		std::vector<std::string> args;
		long int argcnt; 
		for(long int index=0;index < (long int)tokens.size(); index++){
			if(tokens[index].back() == '(' && tokens[index] != "("){
				for(argcnt=index - (long int)argcount(tokens.at(index)); argcnt <= index; argcnt++){
					if(tokens.at(argcnt) != "SEP"){
						temp.push_back(tokens.at(argcnt));
					}
				}
				temp = call(temp);
				//dbg::printvec<std::string>(temp);
				//std::cout << " tempvec\n";
				output.insert(output.end(),temp.begin(),temp.end());
				temp.clear();
				clearfuncdata(tokens[index]);
			}else{
				output.push_back(tokens[index]);
				//dbg::printvec<std::string>(output);
			}	
			//std::cout << "vec\n";
		}
		return output;
	}

	std::vector<std::string> recurselink(std::vector<std::string> tokens, long long int recursecount){

		std::vector<std::string> prev = link(tokens);
		if(!(isEqual(prev, tokens) || recursecount >= recursedepth)){
			return recurselink(prev, recursecount);
			recursecount++;
		}
		return tokens;
		
		
	}
}
