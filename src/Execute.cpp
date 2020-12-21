#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<limits>
#include<sstream>
#include<iostream>
#include<iomanip>

#include "Execute.hpp"
#include "Function.hpp"
#include "Status.hpp"
#include "Token.hpp"
#include "Zetacompiler.hpp"



namespace xmath {

	long long int maxRecurse = 1536;
	long long int recursionCount = 0;

	void setmaxrecurse(long long maxR){
		maxRecurse = maxR;
		return;
	}

	long long getmaxrecurse(void){
		return maxRecurse;
	}


	std::string to_string_hprec(double x) {
		std::ostringstream doublestring;
		doublestring << std::setprecision(std::numeric_limits<double>::digits10) << x;
		return doublestring.str();
	}

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

	double eql(double x, double y){
		if(x == y) return 1;
		return 0;
	}

	double nql(double x, double y){
		if(x != y) return 1;
		return 0;
	}

	double gql(double x, double y){
		if(x >= y) return 1;
		return 0;
	}

	double lql(double x, double y){
		if(x <= y) return 1;
		return 0;
	}	

	double grt(double x, double y){
		if(x > y) return 1;
		return 0;
	}

	double lst(double x, double y){
		if(x < y) return 1;
		return 0;
	}

	double neg(double x){
		return x*-1;
	}

	long long factorial(long long x){
		long long ans=1;
		for(;x>0;x--){
			ans *= x;
		}
		return ans;
	}

	std::string calculate(std::vector<token> tokens, bool showprogress){
		if(recursionCount >= maxRecurse){
			std::vector<token>().swap(tokens);
			std::string error = "Maximum recursion depth exceeded (limit=";
			error.append(std::to_string(recursionCount)).append(")");
			recursionCount = 0;
			throw error;
		}
		recursionCount++;
		bool forcerun = false;
		unsigned long int index = 0;
		long int argtracker = 0;
		long int farg_max;
		if(tokens.size() <= 1 && tokens.front().type != 4){
			return tokens.front().data;
		}else if(tokens.size() == 1){
			forcerun = true;
		}
		/*
		X	0 - NUM
		-	1 - OPERATOR
		X	2 - LEFT BRACKET
		X	3 - RIGHT BRACKET
		-	4 - FUNCTION
		X	5 - VARIABLE
		-	6 - R FUNC
		-	7 - Separator
		*/	
		std::vector<token> argpack; // To be used in functions
		// Cannot use recursion because in certain cases, it will lead to stack overflow
		while(tokens.size() > 1 || forcerun){
			if(showprogress) bar::setcount((float)tokens.size());
			// if(index > tokens.size()){
			// 	index = 0;
			// }
			switch(tokens[index].type){
				case 1:
					if(tokens[index].data == "ADD"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(add(x, y)), 0);
						tokens.insert(tokens.begin()+(index-2),1, tk);
						index -= 3;	
					}else if(tokens[index].data == "SUB"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(sub(x, y)), 0);
						tokens.insert(tokens.begin()+(index-2),1, tk);
						index -= 3;	
					}else if(tokens[index].data == "MUL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(mul(x, y)), 0);
						tokens.insert(tokens.begin()+(index-2),1, tk);
						index -= 3;
					}else if(tokens[index].data == "DIV"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(divide(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;
					}else if(tokens[index].data == "MOD"){
						double x = std::stoll(tokens[index-2].data);
						double y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(mod(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "POW"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(power(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "XOR"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(exclusiveOr(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "SHL"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(shl(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
			// comparisons
					}else if(tokens[index].data == "EQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(eql(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "NQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(nql(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "GQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(gql(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "LQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(lql(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "GRT"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(grt(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "LST"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(lst(x, y));
						tk.type = 0;
						tokens.insert(tokens.begin()+(index-2),1,tk);
						index -= 3;	
					}else if(tokens[index].data == "NEG"){
						double x = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-1);
						tokens.erase(tokens.begin()+index-1);
						token tk(to_string_hprec(neg(x)), 0);
						tokens.insert(tokens.begin()+(index-1),1,tk);
						index -= 2;					
					}else if(tokens[index].data == "POS"){
						tokens.erase(tokens.begin()+index);
						index--;					
					}else{
						goto lblend; // Error
					}
				break;
			case 4:{ // Standard Function
				std::string funcname = tokens.at(index).data;
				if(!fexists(funcname)){
					std::string error = "No matching function call to \"";
					error.append(funcname).append("\"");
					throw error;
				}
				tokens.erase(tokens.begin()+index);
				index--;
				farg_max = argcount(funcname);
				while(index < tokens.size() && argtracker < farg_max){
					if(tokens.at(index).type == 0){
						argpack.insert(argpack.begin(), tokens.at(index));
						tokens.erase(tokens.begin() + index);
						index--;
						argtracker++;
					}else{
						break;
					}
				}
				token tk(calculate(comp::fillallvars(call(argpack, funcname)), false), 0);
				tokens.insert(tokens.begin()+index + 1,1, tk);
				std::vector<token>().swap(argpack);
				argtracker = 0;
				}break;
			case 6:{ // Right Function && single arg
				if(tokens[index].data == "FACT"){
					double x = std::stod(tokens[index-1].data);
					tokens.erase(tokens.begin()+index-1);
					tokens.erase(tokens.begin()+index-1);
					token tk;
					tk.data = to_string_hprec(factorial((long long)x));
					tk.type = 0;
					tokens.insert(tokens.begin()+(index-1),1,tk);	
					index -= 2;	
					}		
				}break;
			case 7:{
				tokens.erase(tokens.begin()+index);
			}break;
			default:{
				index++;
				if(index > tokens.size()){
					goto lblend;
				}
				}break;
			}
			if(forcerun || index > tokens.size()){
				goto lblend;
			}
		}
		lblend:
		recursionCount = 0;
		return tokens.front().data;
	}
}
