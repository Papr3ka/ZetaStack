#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<gmpxx.h>

#include "Execute.hpp"

namespace xmath {
	bool string_isnum(std::string str){
		if(str.size() <= 1){
			if(isdigit(str[0]) || str[0] == '.'){
				return true;
			}else{
				return false;
			}
		}else{ // eg -. would be considerd as -0
			if(isdigit(str[0]) || isdigit(str[1]) || str[0] == '.' || str[1] == '.'){
				return true;
			}else{
				return false;
			}
		}
	}

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

	double floordiv(double x, double y){
		return floor(x / y);
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

	std::string calculate(std::vector<std::string> tokens){
		unsigned long int index = 0;

		// Cannot use recursion because in certain cases, it will lead to stack overflow
		while(tokens.size() > 1){
			index++;
			if(index > tokens.size()){
				index = 0;
			}
			if(!string_isnum(tokens[index])){
				if(tokens[index] == "ADD"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(add(x, y)));
						index = 0;	
				}else if(tokens[index] == "SUB"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(sub(x, y)));
						index = 0;	
				}else if(tokens[index] == "MUL"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(mul(x, y)));
						index = 0;
				}else if(tokens[index] == "DIV"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(divide(x, y)));
						index = 0;
				}else if(tokens[index] == "MOD"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(mod(x, y)));
						index = 0;	
				}else if(tokens[index] == "POW"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(power(x, y)));
						index = 0;	
				}else if(tokens[index] == "XOR"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(exclusiveOr((long long)x, (long long)y)));
						index = 0;	
				}else if(tokens[index] == "FLOORDIV"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(floordiv(x, y)));
						index = 0;	
				}else if(tokens[index] == "SHL"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(shl((long long)x, (long long)y)));
						index = 0;	
				}else if(tokens[index] == "SHR"){
						double x = std::stod(tokens[index-2]);
						double y = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.insert(tokens.begin()+(index-2),1,std::to_string(shr((long long)x, (long long)y)));
						index = 0;	
				}else if(tokens[index] == "FACT"){
						double x = std::stoll(tokens[index-1]);
						tokens.erase(tokens.begin()+index-1);
						tokens.erase(tokens.begin()+index-1);
						tokens.insert(tokens.begin()+(index-1),1,std::to_string(factorial(x)));
						index = 0;	
				}else if(tokens[index] == "sin("){
						double x = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-1);
						tokens.erase(tokens.begin()+index-1);
						tokens.insert(tokens.begin()+(index-1),1,std::to_string(sin(x)));
						index = 0;	
				}else if(tokens[index] == "cos("){
						double x = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-1);
						tokens.erase(tokens.begin()+index-1);
						tokens.insert(tokens.begin()+(index-1),1,std::to_string(cos(x)));
						index = 0;	
				}else if(tokens[index] == "tan("){
						double x = std::stod(tokens[index-1]);
						tokens.erase(tokens.begin()+index-1);
						tokens.erase(tokens.begin()+index-1);
						tokens.insert(tokens.begin()+(index-1),1,std::to_string(tan(x)));
						index = 0;	
				}else {
					break; // Error
				}
			}
		}
		return tokens[0];
	}
}
