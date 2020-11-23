#include<string>
#include<vector>
#include<cctype>
#include<cstdlib>

#include "Preprocessor.hpp"

std::vector<std::string> split(std::string str, std::string split){
    size_t start = 0, end, splitLen = split.size();
    std::string token;
    std::vector<std::string> output;
    while((end = str.find(split, start)) != std::string::npos){
        token = str.substr(start, end - start);
        start = end + splitLen;
        output.push_back(token);
    }
    output.push_back(str.substr(start));
    return output;
}


namespace comp {

	std::vector<std::string> ops {
		"<<=",
		">>=",
		"**=",
		"+=",
		"-=",
		"*=",
		"/=",
		"^=",
		"%="
		"==",
		"!=",
		">=",
		"<=",
		">",
		"<",
		"="
	};


	/*
		-1 Error
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
	int assigntype(std::string str){
		size_t eqpos = str.find('=');
		if(eqpos == std::string::npos){
			return -1;
		}else{
			int ieqpos = static_cast<int>(eqpos);
			if(ieqpos - 2 >= 0){
				if(str[ieqpos - 2] == '<' && str[ieqpos - 1] == '<'){
					return 8;
				}else if(str[ieqpos - 2] == '>' && str[ieqpos - 1] == '>'){
					return 9;
				}else if(str[ieqpos - 2] == '*' && str[ieqpos - 1] == '*'){
					return 7;
				}else{

				}
			}
			if(ieqpos - 1 >= 0){
				if(str[ieqpos - 1] == '+'){
					return 1;
				}else if(str[ieqpos - 1] == '-'){
					return 2;
				}else if(str[ieqpos - 1] == '*'){
					return 3;
				}else if(str[ieqpos - 1] == '/'){
					return 4;
				}else if(str[ieqpos - 1] == '%'){
					return 5;
				}else if(str[ieqpos - 1] == '^'){
					return 6;
				}else{

				}
			}
		}
		return 0;
	}


	std::string removeWhiteSpace(std::string str){
		str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
		return str;
	}

	std::string stripcomment(std::string str){
		bool take = true;
		std::string out;
		for(unsigned long int idx = 0; idx < str.size(); idx++){
			if(idx+1 < str.size()){
				if(str[idx] == '/' && str[idx + 1] == '*'){
					take = false;
				}else if(str[idx - 1] == '*' && str[idx] == '/'){
					take = true;
				}else{
					if(take){
						out.push_back(str[idx]);
					}
				}
			}else{
				if(take){
					out.push_back(str[idx]);
				}
			}

		}
		return out;
	}


	/*
		-1 Error
		0 Calculate
		1 Assign
		2 Bool Compare
		3 Calculate-Assign
	*/
	int execType(std::string str){
		size_t eqpos = str.find('=');
		if(eqpos == std::string::npos){
			return 0;
		}else{
			int ieqpos = static_cast<int>(eqpos);
			if(ieqpos - 1 > 0){
				// Assign + operation
				if(str[ieqpos-1] == '<' && str[ieqpos] == '<'){
					return 3;
				}else if(str[ieqpos-1] == '>' && str[ieqpos] == '>'){
					return 3;
				}else if(str[ieqpos-1] == '*' && str[ieqpos] == '*'){
					return 3;
				}else if(str[ieqpos-1] == '/' && str[ieqpos] == '/'){
					return 3;
				}else{

				}
			}
		
			if(str[ieqpos] == '='){
				return 1;
			}else if(str[ieqpos] == '>'){
				return 2;
			}else if(str[ieqpos] == '<'){
				return 2;
			}else if(str[ieqpos] == '!'){
				return 2;
			}else if(str[ieqpos] == '+'){
				return 3;
			}else if(str[ieqpos] == '-'){
				return 3;
			}else if(str[ieqpos] == '*'){
				return 3;
			}else if(str[ieqpos] == '/'){
				return 3;
			}else if(str[ieqpos] == '%'){
				return 3;
			}else if(str[ieqpos] == '^'){
				return 3;
			}else{
				return -1;
			}
			
		}
	}

	std::vector<std::string> spliteq(std::string str){
		std::vector<std::string> outvec{str};
		for(auto x: ops){
			outvec = split(str, x);
			if(outvec.size() >= 2){
				return outvec;
			}
		}
		return outvec;
	}


}