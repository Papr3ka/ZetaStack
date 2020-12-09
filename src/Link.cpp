#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>

#include "Link.hpp"
#include "Function.hpp"
#include "Token.hpp"

long long int recursedepth = 4096; // Default 4096

namespace ld {

	static bool isEqual(std::vector<token> fcompvec, std::vector<token> bcompvec)
	{
	    if(fcompvec.size() != bcompvec.size()){
	    	return false;
	    }else{
	    	for(unsigned long int i = 0; i < fcompvec.size(); i++){
	    		if(fcompvec[i] != bcompvec[i]){
	    			return false;
	    		}
	    	}
	    }
	    return true;
	}
 

	// Do one round of function linking
	std::vector<token> link(std::vector<token> tokens){
		std::string funcname;
		std::vector<token> output;
		std::vector<token> temp;
		std::vector<token> temppack;
		std::vector< std::vector<token> > argpackage;
		long int braccounter;
		long int tpos = 0;
		while(!tokens.empty()){
			if(tokens.front().type == 4){
				braccounter = 1;
				funcname = tokens.front().data;
				tokens.erase(tokens.begin());
				tokens.erase(tokens.begin());
				token tk;
				tk.data = "L_BRAC";
				tk.type = 2;
				output.push_back(tk);
				while(braccounter != 0 && !tokens.empty()){
					std::cout << tokens.front().data << ", ";
					if(tokens.front().type == 2){
						braccounter++;
						token tk;
						tk.data = "L_BRAC";
						tk.type = 2;
						temppack.push_back(tk);
						tokens.erase(tokens.begin());
						tpos--;
					}else if(tokens.front().type == 3){
						braccounter--;
						token tk;
						tk.data = "R_BRAC";
						tk.type = 3;
						temppack.push_back(tk);
						tokens.erase(tokens.begin());
						tpos--;
					}else{
						output.push_back(tokens.front());
						tokens.erase(tokens.begin());	
						tpos--;					
					}
					if(tokens.empty()) break;
					if(tokens.front().type == -1 || tokens.empty()){
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

	std::vector<token> recurselink(std::vector<token> tokens, long long int recursecount){
		std::vector<token> prev = link(tokens);
		while(!isEqual(prev,tokens) && recursecount > 0){
			prev = tokens;
			tokens = link(tokens);
			recursecount--;
		}
		return tokens;


	}
}
