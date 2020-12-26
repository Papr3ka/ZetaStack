#include<cmath>
#include<cstdlib>
#include<iomanip>
#include<iostream>
#include<limits>
#include<sstream>
#include<string>
#include<vector>

#include "Cache.hpp"
#include "Execute.hpp"
#include "Function.hpp"
#include "Status.hpp"
#include "Token.hpp"
#include "Variable.hpp"
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


	inline std::string to_string_hprec(double x){
		std::ostringstream doublestring;
		doublestring << std::setprecision(std::numeric_limits<double>::digits10) << x;
		return doublestring.str();
	}

	void resetsstreamsettings(void){
		std::stringstream strstream(std::stringstream::in | std::stringstream::out);
		strstream << std::setprecision(6);
		return;
	}	

	inline std::string to_string_hprec(long long x){
		std::ostringstream llstring;
		llstring << std::setprecision(std::numeric_limits<long long>::digits10) << x;
		return llstring.str();
	}	

	inline bool string_isnum(std::string str){
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
	inline double add(double x, double y){
		return x+y;
	}

	inline double sub(double x, double y){
		return x-y;
	}

	inline double mul(double x, double y){
		return x*y;
	}

	inline double divide(double x, double y){
		return x/y;
	}

	inline double mod(double x, double y){
		return fmod(x,y);
	}

	inline double power(double x, double y){
		return pow(x, y);
	}

	inline long long bitexclusiveOr(long long x, long long y){
		return x ^ y;
	}

	inline long long shl(long long x, long long y){
		return x << y;
	}

	inline long long shr(long long x, long long y){
		return x >> y;
	}

	inline double eql(double x, double y){
		if(x == y) return 1;
		return 0;
	}

	inline double nql(double x, double y){
		if(x != y) return 1;
		return 0;
	}

	inline double gql(double x, double y){
		if(x >= y) return 1;
		return 0;
	}

	inline double lql(double x, double y){
		if(x <= y) return 1;
		return 0;
	}	

	inline double grt(double x, double y){
		if(x > y) return 1;
		return 0;
	}

	inline double lst(double x, double y){
		if(x < y) return 1;
		return 0;
	}

	inline double neg(double x){
		return x*-1;
	}

	inline long long bitwor(long long x, long long y){
		return x | y;
	}

	inline long long bitwand(long long x, long long y){
		return x & y;
	}

	inline double land(double x, double y){
		return x && y;
	}

	inline double lor(double x, double y){
		return x || y;
	}

	inline long long factorial(long long x){
		long long ans=1;
		for(;x>0;x--){
			ans *= x;
		}
		return ans;
	}

	/*
	0 ASN     =
	1 ADDASN  +=
	2 SUBASN  -=
	3 MULASN  *=
	4 DIVASN  /=
	5 MODASN  %=
	6 XORASN  ^=
	7 POWASN  **=
	8 SHLASN  <<=
	9 SHRASN  >>=
	*/

	inline void sasn(std::string iden, std::string val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden, val);
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}		
	}

	inline void asn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(val));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void addasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(add(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void subasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(sub(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void mulasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(mul(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void divasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(divide(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void modasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(mod(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void exclusiveOrasn(std::string iden, long long val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(bitexclusiveOr(std::stoll(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void powasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(pow(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void shlasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(shl(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void shrasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(shr(std::stod(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	inline void bitexclusiveOrasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(bitexclusiveOr(std::stoll(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}	

	inline void bitorasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(bitwor(std::stoll(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}	

	inline void bitandasn(std::string iden, double val){
		cch::add_depend(iden);
		if(var::changable(iden)){
			var::update(iden,to_string_hprec(bitwand(std::stoll(var::search(iden)), val)));
			return;
		}else{
			std::string error = "Variable \"";
			error.append(iden).append("\" is read-only");
			throw error;		
		}
	}

	std::string calculate(std::vector<token> tokens, bool showprogress){
		if(recursionCount >= maxRecurse){
			std::vector<token>().swap(tokens);
			std::string error = "Maximum recursion depth exceeded (limit=";
			error.append(std::to_string(recursionCount)).append(")");
			recursionCount = 0;
			throw error;
		}
		const token first = tokens.front();
		recursionCount++;
		unsigned long int index = 0;
		long int argtracker = 0;
		long int farg_max;

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
		goto forceRun; // At least run through once loop
		while(tokens.size() > 1){
			forceRun:
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
						token tk(to_string_hprec(add(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "SUB"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(sub(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "MUL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(mul(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;
					}else if(tokens[index].data == "DIV"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(divide(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;
					}else if(tokens[index].data == "MOD"){
						double x = std::stoll(tokens[index-2].data);
						double y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(mod(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "POW"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(power(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "BITXOR"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(bitexclusiveOr(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "BITOR"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(bitwor(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "BITAND"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(bitwand(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;
					}else if(tokens[index].data == "LAND"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk(to_string_hprec(land(x, y)), 0);
						tokens.at(index-2) = tk;
						index -= 3;
					}else if(tokens[index].data == "LOR"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(lor(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;
					}else if(tokens[index].data == "SHL"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(shl(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "SHR"){
						long long x = std::stoll(tokens[index-2].data);
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(shr(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;
			// comparisons
					}else if(tokens[index].data == "EQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(eql(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "NQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(nql(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "GQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(gql(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "LQL"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(lql(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "GRT"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(grt(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
						index -= 3;	
					}else if(tokens[index].data == "LST"){
						double x = std::stod(tokens[index-2].data);
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						token tk;
						tk.data = to_string_hprec(lst(x, y));
						tk.type = 0;
						tokens.at(index-2) = tk;
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

			case 10:{
					if(tokens[index].data == "ASN"){
						if(tokens[index - 1].type == 9){
							std::string idenx = tokens[index-2].data;
							std::string strval = tokens[index-1].data;
							tokens.erase(tokens.begin()+index-2);
							tokens.erase(tokens.begin()+index-2);
							tokens.erase(tokens.begin()+index-2);
							sasn(idenx, strval);
							index -= 3;	
						}else{
							std::string idenx = tokens[index-2].data;
							double y = std::stod(tokens[index-1].data);
							tokens.erase(tokens.begin()+index-2);
							tokens.erase(tokens.begin()+index-2);
							tokens.erase(tokens.begin()+index-2);
							asn(idenx, y);
							index -= 3;			
						}
					}else if(tokens[index].data == "ADDASN"){
						std::string idenx = tokens[index-2].data;
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						addasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "SUBASN"){
						std::string idenx = tokens[index-2].data;
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						subasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "MULASN"){
						std::string idenx = tokens[index-2].data;
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						mulasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "DIVASN"){
						std::string idenx = tokens[index-2].data;
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						divasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "MODASN"){
						std::string idenx = tokens[index-2].data;
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						modasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "BITXORASN"){
						std::string idenx = tokens[index-2].data;
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						bitexclusiveOrasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "BITORASN"){
						std::string idenx = tokens[index-2].data;
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						bitorasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "BITANDASN"){
						std::string idenx = tokens[index-2].data;
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						bitandasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "POWASN"){
						std::string idenx = tokens[index-2].data;
						double y = std::stod(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						powasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "SHLASN"){
						std::string idenx = tokens[index-2].data;
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						shlasn(idenx, y);
						index -= 3;	
					}else if(tokens[index].data == "SHRASN"){
						std::string idenx = tokens[index-2].data;
						long long y = std::stoll(tokens[index-1].data);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						tokens.erase(tokens.begin()+index-2);
						shrasn(idenx, y);
						index -= 3;	
					}else{
						goto lblend;
					}
				}break;
			default:{
				index++;
				if(index > tokens.size()){
					goto lblend;
				}
				}break;
			}
		}
		lblend:
		recursionCount = 0;
		if(tokens.size() <= 0){
			return var::mostrecent();
		}else if(first.type != 0 && first == tokens.front()){
			std::string error = "Unexpected Token";
			throw error;
		} 
		return tokens.front().data;
		
	}
}
