#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>

#include<gmpxx.h>

#include "Execute.hpp"
#include "Function.hpp"
#include "Status.hpp"
#include "Token.hpp"

namespace xmath {

	bool string_isnum(std::string str){
		if(str.size() <= 1){
			if(isdigit(str[0]) || str[0] == '.'){
				return true;
			}else{
				return false;
			}
		}else{ // eg -. would be considerd as -0
			if(isdigit(str[0]) || 
			   isdigit(str[1]) || 
			     str[0] == '.' || 
			     str[1] == '.'){
				return true;
			}else{
				return false;
			}
		}
	}

	// Placeholder functions for Arbitrary math
	double add(double x, double y){
		return x+y;
	}

	double sub(double x, double y){
		return x-y;
	}

	double mul(double x, double y){
		return x*y;
	}

	double divide(double x, double y){
		return x/y;
	}

	double mod(double x, double y){
		return fmod(x,y);
	}

	double power(double x, double y){
		return pow(x, y);
	}

	long long exclusiveOr(long long x, long long y){
		return x ^ y;
	}

	long long shl(long long x, long long y){
		return x << y;
	}

	long long shr(long long x, long long y){
		return x >> y;
	}

	long long factorial(long long x){
		long long ans=1;
		for(;x>0;x--){
			ans *= x;
		}
		return ans;
	}

	std::string calculate(std::vector<token> tokens, bool showprogress){
		unsigned long int index = 0;
		if(tokens.size() <= 1){
			return tokens.front().data;
		}
		// Cannot use recursion because in certain cases, it will lead to stack overflow
		while(tokens.size() > 1){
			if(showprogress) bar::setcount((float)tokens.size());
			// if(index > tokens.size()){
			// 	index = 0;
			// }
			if(tokens[index].type == 1){
				if(tokens[index].data == "ADD"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(std::to_string(add(x, y)), 0);
						tokens.insert(tokens.begin()+(index-2),1, tk);
						index = 0;	
				}else if(tokens[index].data == "SUB"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(std::to_string(sub(x, y)), 0);
						tokens.insert(tokens.begin()+(index-2),1, tk);
						index = 0;	
				}else if(tokens[index].data == "MUL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(std::to_string(mul(x, y)), 0);
						tokens.insert(tokens.begin()+(index-2),1, tk);
						index = 0;
				}else if(tokens[index].data == "DIV"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = std::to_string(divide(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index = 0;
				}else if(tokens[index].data == "MOD"){
						double x = std::stoll(tokens[index-2].data);
						double y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = std::to_string(mod(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index = 0;	
				}else if(tokens[index].data == "POW"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = std::to_string(power(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index = 0;	
				}else if(tokens[index].data == "XOR"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = std::to_string(exclusiveOr(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index = 0;	
				}else if(tokens[index].data == "SHL"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = std::to_string(shl(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index = 0;	
				}else if(tokens[index].data == "SHR"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = std::to_string(shr(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index = 0;	
				}else if(tokens[index].data == "FACT"){
					double x = std::stod(tokens[index-1].data);
					tokens.erase(tokens.begin()+index-1);
					tokens.erase(tokens.begin()+index-1);
					token tk;
					tk.data = std::to_string(factorial((long long)x));
					tk.type = 0;
					tokens.insert(tokens.begin()+(index-1),1,tk);		
				}else{
					break; // Error
				}
			}else{
				index++;
				if(index > tokens.size()){
					break;
				}
			}
		}
		return tokens.front().data;
	}

	
	// now done in Link.cpp

	// // For debugging purposes
	// void printvec(std::vector<std::string> print){
	// 	std::cout << '[';
	// 	for(unsigned long i=0; i < print.size(); i++){
	// 		if(i >= 1){
	// 			std::cout << ", ";
	// 		}
	//    		std::cout << print.at(i);
	// 	}
	// 	std::cout << "]\n";
	// }
	


	// std::vector<std::string> callall(std::vector<std::string> tokens){
	// 	std::vector<std::string> arguments;
	// 	std::vector<std::string> output;
	// 	unsigned long int argcnt;
	// 	unsigned long int startdel;
	// 	unsigned long int index = 0;
	// 	unsigned long int reachsize = tokens.size();
	// 	while(index < reachsize){
	// 		std::cout << index << " i\n";
	// 		if(tokens.at(index).back() == '('){
	// 			argcnt = argcount(tokens.at(index));
	// 			startdel = index - argcnt;
	// 			for(unsigned long int subindex = index - argcnt; subindex <= argcnt; subindex++){
	// 				arguments.push_back(tokens[subindex]); // Pack args
	// 			}
	// 			printvec(output);
	// 			std::cout << " out\n";
	// 			for(unsigned long int subindex = index - argcnt; subindex < argcnt; subindex++){
	// 				output.erase(output.begin()+startdel); // Delete args
	// 			}
	// 			printvec(output);
	// 			std::cout << " out\n";
	// 			printvec(arguments);
	// 			reachsize -= arguments.size() - 1;
	// 			index -= arguments.size() - 1;
	// 			printvec(output);
	// 			output.push_back(calculate(call(arguments)));
	// 			printvec(output);
	// 			arguments.clear();
	// 		}else{
	// 			output.push_back(tokens.at(index));
	// 		}
	// 		index++;
			
	// 	}
		
	// 	return output;
	// }
}
