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
				}else if(lexInput[index] == '>'){
					index++;
					returnedTokens.push_back(">");
				}else if(lexInput[index] == '<'){
					index++;
					returnedTokens.push_back("<");
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

/*
	0 - NUM
	1 - OPERATOR
	2 - LEFT BRACKET
	3 - RIGHT BRACKET
	4 - FUNCTION
	5 - VARIABLE
	6 - R FUNC
	7 - Separator / Func End
*/	

	std::vector<token> tokenComp(std::vector<std::string> tokensInput){
		std::vector<token> output;
		for(unsigned long int index = 0;index<tokensInput.size();index++){
			if(!isdigit(tokensInput[index][0])){
				if(tokensInput[index] == ","){
					token tk("SEP",7);
					output.push_back(tk);
				}else if(tokensInput[index] == "+"){
					token tk("ADD",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "-"){
					token tk("SUB",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "*"){
					token tk("MUL",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "/"){
					token tk("DIV",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "%"){
					token tk("MOD",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "^"){
					token tk("XOR",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "!"){
					token tk("FACT",6);
					output.push_back(tk);
				}else if(tokensInput[index] == "("){
					token tk("L_BRAC",2);
					output.push_back(tk);
				}else if(tokensInput[index] == ")"){
					token tk("R_BRAC",3);
					output.push_back(tk);
				}else if(tokensInput[index] == "="){
					token tk("ASN",10);
					output.push_back(tk); // Assign
				}else if(tokensInput[index] == "**"){
					token tk("POW",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "//"){
					token tk("FLOORDIV",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "||"){
					token tk("OR",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "&&"){
					token tk("AND",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "<<"){
					token tk("SHL",1);
					output.push_back(tk);
				}else if(tokensInput[index] == ">>"){
					token tk("SHR",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "=="){
					token tk("EQL",1);
					output.push_back(tk); // Equal
				}else if(tokensInput[index] == "!="){
					token tk("NQL",1);
					output.push_back(tk);
				}else if(tokensInput[index] == ">="){
					token tk("GQL",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "<="){
					token tk("LQL",1);
					output.push_back(tk);
				}else if(tokensInput[index] == ">"){
					token tk("GRT",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "<"){
					token tk("LST",1);
					output.push_back(tk);
				}else if(tokensInput[index] == "+="){
					token tk("ADDASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "-="){
					token tk("SUBASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "*="){
					token tk("MULASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "/="){
					token tk("DIVASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "^="){
					token tk("XORASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "<<="){
					token tk("SHLASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == ">>="){
					token tk("SHRASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "**="){
					token tk("POWASN",10);
					output.push_back(tk);
				}else if(tokensInput[index] == "//="){
					token tk("FLOORDIVASN",10);
					output.push_back(tk);
				}else if(tokensInput[index][0] == '.'){
					token tk(tokensInput[index],0);
					output.push_back(tk);
				}else if(isalpha(tokensInput[index].back())){
					token tk(tokensInput[index],5);
					output.push_back(tk);
				}else if(tokensInput[index].back() == '('){
					token tk(tokensInput[index],4);
					output.push_back(tk);
				}else{
					token tk("NULL",-1);
					output.push_back(tk);
				}
			}else if(tokensInput[index].back() == '('){
				token tk(tokensInput[index],4);
				output.push_back(tk);
			}else if(ttype(tokensInput[index]) == 0){
				token tk(tokensInput[index],0);
				output.push_back(tk);			
			}else{
				token tk(tokensInput[index],5);
				output.push_back(tk);
			}
		}
		unsigned long int replidx = 1;
		if(output.front().type == 1 && output.front().data == "SUB"){
			output.at(0).data = "NEG"; 
			output.at(0).type = 1;			
		}else if(output.front().type == 1 && output.front().data == "ADD"){
			output.at(0).data = "POS"; 
			output.at(0).type = 1;				
		}
		if(output.size() >= 2){
			if(output.front().type == 1 && (output.front().data == "SUB" || output.front().data == "ADD")){
				if(output.at(1).type == 0 || output.at(1).type == 5){
					if(output.front().data == "SUB"){
						output.at(0).data = "NEG"; 
						output.at(0).type = 1;
					}else if(output.front().data == "ADD"){
						output.at(0).data = "POS"; 
						output.at(0).type = 1;						
					}
				}
			}
		}
		while(replidx < output.size()){
			if(replidx + 1 < output.size() && output.at(replidx).type == 1 &&
			 (output.at(replidx).data == "SUB" || output.at(replidx).data == "ADD")){
				if((output.at(replidx + 1).type == 2 || 
					output.at(replidx + 1).type == 1 || 
					output.at(replidx - 1).type == 2 || 
					output.at(replidx - 1).type == 1) &&
					output.at(replidx - 1).type != 0 &&
					output.at(replidx - 1).type != 5){
					if(output.at(replidx).data == "SUB"){
						output.at(replidx).data = "NEG"; 
						output.at(replidx).type = 1;
					}else if(output.at(replidx).data == "ADD"){
						output.at(replidx).data = "POS"; 
						output.at(replidx).type = 1;						
					}
				}
			}
			replidx++;
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
			long int ieqpos = static_cast<long int>(eqpos);
			if(ieqpos - 2 >= 0){
				if(str[ieqpos - 2] == '>' || str[ieqpos - 2] == '<' || str[ieqpos - 2] == '*'){
					return 1;
				}
			}
			if(ieqpos - 1 >= 0){
				if(str[ieqpos - 1] == '>' || str[ieqpos - 1] == '<' || str[ieqpos - 1] == '!'){
					return 0;
				}			
			}
			if((unsigned long int)ieqpos + 1 < str.size()){
				if(str[ieqpos + 1] == '='){
					return 0;
				}
			}
			if(str[ieqpos] == '='){
				if(fbrac == std::string::npos){
					return 1;
				}else{
					long int fbracpos = static_cast<long int>(fbrac);
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