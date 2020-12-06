#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<iostream>

#include "Preprocessor.hpp"
#include "Zetacompiler.hpp"
#include "Token.hpp"

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

	// Lexical Analysis
	std::vector<std::string> lex(std::string lexInput){
		std::vector<std::string> returnedTokens;
		std::string dualchar;
		unsigned long int loopcount = 0;
		unsigned long int index = 0;
		while(index <= lexInput.size()){
			if(ispunct(lexInput[index]) && lexInput[index] != '.' && lexInput[index] != ','){
				if(index+1 < lexInput.size()){
					dualchar = lexInput.substr(index,2);
					if(dualchar == "**"){
						index += 2;
						returnedTokens.push_back("**");
					}else if(dualchar == "//"){
						index += 2;
						returnedTokens.push_back("//");
					}else if(dualchar == "||"){
						index += 2;
						returnedTokens.push_back("||");
					}else if(dualchar == "&&"){
						index += 2;
						returnedTokens.push_back("&&");
					}else if(dualchar == "<<"){
						index += 2;
						returnedTokens.push_back("<<");
					}else if(dualchar == ">>"){
						index += 2;
						returnedTokens.push_back(">>");
					}else if(dualchar == "=="){
						index += 2;
						returnedTokens.push_back("==");
					}else if(dualchar == "!="){
						index += 2;
						returnedTokens.push_back("!=");
					}else if(dualchar == ">="){
						index += 2;
						returnedTokens.push_back(">=");
					}else if(dualchar == "<="){
						index += 2;
						returnedTokens.push_back("<=");
					}else{
						// error
					}
				}
				if(lexInput[index] == '+'){
					index++;
					returnedTokens.push_back("+");
				}else if(lexInput[index] == '-'){
					index++;
					returnedTokens.push_back("-");
				}else if(lexInput[index] == '*'){
					index++;
					returnedTokens.push_back("*");
				}else if(lexInput[index] == '/'){
					index++;
					returnedTokens.push_back("/");
				}else if(lexInput[index] == '%'){
					index++;
					returnedTokens.push_back("%");
				}else if(lexInput[index] == '^'){
					index++;
					returnedTokens.push_back("^");
				}else if(lexInput[index] == '!'){
					index++;
					returnedTokens.push_back("!");
				}else if(lexInput[index] == '('){
					index++;
					returnedTokens.push_back("(");
				}else if(lexInput[index] == ')'){
					index++;
					returnedTokens.push_back(")");
				}else if(lexInput[index] == '='){
					index++;
					returnedTokens.push_back("=");
				}else{
					//error
				}
				

			}else if(isalpha(lexInput[index])){
				// check if variable first
				unsigned long int countindex = index;
				while(countindex < lexInput.size() && isalpha(lexInput[countindex])){
					countindex++;
				}
				if(lexInput[countindex] == '('){
					// if functions call
					returnedTokens.push_back(lexInput.substr(index,countindex-index + 1));
					index += countindex-index + 1;
					returnedTokens.push_back("(");
				}else{
					returnedTokens.push_back(lexInput.substr(index,countindex-index));
					index += countindex-index;
				}
			}else if(isdigit(lexInput[index]) || lexInput[index] == '.'){
				unsigned long int countindex = index;
				decimal:
					while(countindex < lexInput.size() && isdigit(lexInput[countindex])){
						countindex++;
					}
					if(lexInput[countindex] == '.'){
						countindex++;
						goto decimal;
					}
				returnedTokens.push_back(lexInput.substr(index,countindex-index));
				index += countindex-index;
			}else if(lexInput[index] == ','){
				returnedTokens.push_back(","); // default sep
				index++;
			}else{
				index++;
			}
			loopcount++;
			if(loopcount >= 2*lexInput.size()){
				break; // Error
			}
		}
		for(unsigned long index = 1; index < returnedTokens.size(); index++){
			if(returnedTokens[index-1] == "*" && returnedTokens[index] == "*="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"**=");
			}else if(returnedTokens[index-1] == "<<" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"<<=");
			}else if(returnedTokens[index-1] == ">>" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,">>=");
			}else if(returnedTokens[index-1] == "+" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"+=");
			}else if(returnedTokens[index-1] == "-" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"-=");
			}else if(returnedTokens[index-1] == "*" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"*=");
			}else if(returnedTokens[index-1] == "/" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"/=");
			}else if(returnedTokens[index-1] == "%" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"%=");
			}else if(returnedTokens[index-1] == "^" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"^=");
			}else if(returnedTokens[index-1] == "=" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"==");
			}else if(returnedTokens[index-1] == "!" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"!=");
			}else if(returnedTokens[index-1] == ">" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,">=");
			}else if(returnedTokens[index-1] == "<" && returnedTokens[index] == "="){
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.erase(returnedTokens.begin()+index-1);
				returnedTokens.insert(returnedTokens.begin()+index-1,1,"<=");
			}else{
				// Error
			}
		}
		return returnedTokens;
	}

	std::vector<token> tokenComp(std::vector<std::string> tokensInput){
		std::vector<token> output;
		for(unsigned long int index = 0;index<tokensInput.size();index++){
			token tk;
			if(!isdigit(tokensInput[index][0])){
				if(tokensInput[index] == ","){
					tk.data = "SEP";
					tk.type = 7;
					output.push_back(tk);
				}else if(tokensInput[index] == "+"){
					tk.data = "ADD";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "-"){
					tk.data = "SUB";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "*"){
					tk.data = "MUL";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "/"){
					tk.data = "DIV";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "%"){
					tk.data = "MOD";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "^"){
					tk.data = "XOR";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "!"){
					tk.data = "FACT";
					tk.type = 6;
					output.push_back(tk);
				}else if(tokensInput[index] == "("){
					tk.data = "L_BRAC";
					tk.type = 2;
					output.push_back(tk);
				}else if(tokensInput[index] == ")"){
					tk.data = "R_BRAC";
					tk.type = 3;
					output.push_back(tk);
				}else if(tokensInput[index] == "="){
					tk.data = "ASN";
					tk.type = 10;
					output.push_back(tk); // Assign
				}else if(tokensInput[index] == "**"){
					tk.data = "POW";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "//"){
					tk.data = "FLOORDIV";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "||"){
					tk.data = "OR";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "&&"){
					tk.data = "AND";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "<<"){
					tk.data = "SHL";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == ">>"){
					tk.data = "SHR";
					tk.type = 1;
					output.push_back(tk);
				}else if(tokensInput[index] == "=="){
					tk.data = "ADD";
					tk.type = 11;
					output.push_back(tk); // Equal
				}else if(tokensInput[index] == "!="){
					tk.data = "NQL";
					tk.type = 11;
					output.push_back(tk);
				}else if(tokensInput[index] == ">="){
					tk.data = "GQL";
					tk.type = 11;
					output.push_back(tk);
				}else if(tokensInput[index] == "<="){
					tk.data = "LQL";
					tk.type = 11;
					output.push_back(tk);
				}else if(tokensInput[index] == "+="){
					tk.data = "ADDASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "-="){
					tk.data = "SUBASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "*="){
					tk.data = "MULASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "/="){
					tk.data = "DIVASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "^="){
					tk.data = "XORASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "<<="){
					tk.data = "SHLASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == ">>="){
					tk.data = "SHRASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "**="){
					tk.data = "POWASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index] == "//="){
					tk.data = "FLOORDIVASN";
					tk.type = 10;
					output.push_back(tk);
				}else if(tokensInput[index][0] == '.'){
					tk.data = tokensInput[index];
					tk.type = 0;
					output.push_back(tk);
				}else if(isalpha(tokensInput[index][0])){
					tk.data = tokensInput[index];
					tk.type = 5;
					output.push_back(tk);
				}else{
					tk.data = "NULL";
					tk.type = -1;
					output.push_back(tk);
				}
			}else if(tokensInput[index].back() == '('){
				tk.data = tokensInput[index];
				tk.type = 4;
				output.push_back(tk);
			}else if(ttype(tokensInput[index]) == 0){
				tk.data = tokensInput[index];
				tk.type = 0;
				output.push_back(tk);			
			}else{
				tk.data = tokensInput[index];
				tk.type = 5;
				output.push_back(tk);
			}
		}
		if(output.size() > 1){
			if(output[0].data == "SUB" && output[0].type == 1){			
				output.erase(output.begin());
				output[0].data.insert(output[0].data.begin(),1,'-');
				//std::cout << output[0] << "\n";
			}
		}
		// Unary Sign
		for(unsigned long int idx=1; idx < output.size() - 1; idx++){
			if(output[idx - 1].type == 1 && output[idx].data == "SUB" && output[idx + 1].type == 0){
				output.erase(output.begin() + idx);
				if(output[idx].data.front() == '+' || output[idx].data.front()== '-'){
					output[idx].data[0] = '-';
				}else{
					output[idx].data.insert(output[idx].data.begin(),1,'-');
				}
			}else if(output[idx - 1].type == 1 && output[idx].data == "ADD" && output[idx + 1].type == 0){
				output.erase(output.begin() + idx);
				if(output[idx].data.front() == '+' || output[idx].data.front() == '-'){
					output[idx].data[0] = '+';
				}else{
					output[idx].data.insert(output[idx].data.begin(),1,'+');
				}
			}else{
				// Error
			}
		}
		return output;
	}


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
		0 Calculate
		1 Assign
		2 Assign Function
	*/
	int execType(std::string str){
		size_t fbrac = str.find('(');
		size_t eqpos = str.find('=');
		if(eqpos == std::string::npos){
			return 0;
		}else{
			unsigned long int ieqpos = static_cast<unsigned long int>(eqpos);
			if(str[ieqpos] == '='){
				if(fbrac == std::string::npos){
					return 1;
				}else{
					unsigned long int fbracpos =  static_cast<unsigned long int>(fbrac);
					if(fbracpos < ieqpos){
						return 2;
					}else{
						return 1;
					}
				}
			}
			
		}
		return -1;
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