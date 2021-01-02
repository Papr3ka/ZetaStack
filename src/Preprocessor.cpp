#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<iostream>
#include<string>
#include<vector>

#include "Preprocessor.hpp"
#include "Token.hpp"
#include "Zetacompiler.hpp"

std::vector<std::string> split(std::string str, std::string split){
    size_t start = 0, end, splitLen = split.size();
    std::string token;
    std::vector<std::string> output;
    while((end = str.find(split, start)) != std::string::npos){
        token = str.substr(start, end - start);
        start = end + splitLen;
        output.emplace_back(token);
    }
    output.emplace_back(str.substr(start));
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
		"%=",
		"|=",
		"&=",
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
		std::string trichar;
		unsigned long int loopcount = 0;
		unsigned long int index = 0;
		unsigned long int countindex;

		while(index <= lexInput.size()){
			if(lexInput[index] == '"'){
				countindex = index;
				while(index < lexInput.size()){
					index++;
					if(lexInput[index] == '"' && lexInput[index - 1] != '\\'){
						index++;
						break;
					}
				}
				returnedTokens.emplace_back(lexInput.substr(countindex, index));
				if(returnedTokens.back().back() == ' '){
					returnedTokens.back().pop_back();
				}
			}else if(lexInput[index] == '\''){
				countindex = index;
				while(index < lexInput.size()){
					index++;
					if(lexInput[index] == '\'' && lexInput[index - 1] != '\\'){
						index++;
						break;
					}
				}
				returnedTokens.emplace_back(lexInput.substr(countindex, index));
				if(returnedTokens.back().back() == ' '){
					returnedTokens.back().pop_back();
				}
			}else 
			if(ispunct(lexInput[index]) && lexInput[index] != '.' && lexInput[index] != ','){
				if(index+1 < lexInput.size()){
					dualchar = lexInput.substr(index,2);
					if(dualchar == "**"){
						index += 2;
						returnedTokens.emplace_back("**");
					}else if(dualchar == "//"){
						index += 2;
						returnedTokens.emplace_back("//");
					}else if(dualchar == "||"){
						index += 2;
						returnedTokens.emplace_back("||");
					}else if(dualchar == "&&"){
						index += 2;
						returnedTokens.emplace_back("&&");
					}else if(dualchar == "<<"){
						index += 2;
						returnedTokens.emplace_back("<<");
					}else if(dualchar == ">>"){
						index += 2;
						returnedTokens.emplace_back(">>");
					}else if(dualchar == "=="){
						index += 2;
						returnedTokens.emplace_back("==");
					}else if(dualchar == "!="){
						index += 2;
						returnedTokens.emplace_back("!=");
					}else if(dualchar == ">="){
						index += 2;
						returnedTokens.emplace_back(">=");
					}else if(dualchar == "<="){
						index += 2;
						returnedTokens.emplace_back("<=");
					}else{
						loopcount++;
						// error
					}
				}
				if(lexInput[index] == '+'){
					index++;
					returnedTokens.emplace_back("+");
				}else if(lexInput[index] == '-'){
					index++;
					returnedTokens.emplace_back("-");
				}else if(lexInput[index] == '*'){
					index++;
					returnedTokens.emplace_back("*");
				}else if(lexInput[index] == '/'){
					index++;
					returnedTokens.emplace_back("/");
				}else if(lexInput[index] == '%'){
					index++;
					returnedTokens.emplace_back("%");
				}else if(lexInput[index] == '^'){
					index++;
					returnedTokens.emplace_back("^");
				}else if(lexInput[index] == '!'){
					index++;
					returnedTokens.emplace_back("!");
				}else if(lexInput[index] == '('){
					index++;
					returnedTokens.emplace_back("(");
				}else if(lexInput[index] == ')'){
					index++;
					returnedTokens.emplace_back(")");
				}else if(lexInput[index] == '='){
					index++;
					returnedTokens.emplace_back("=");
				}else if(lexInput[index] == '>'){
					index++;
					returnedTokens.emplace_back(">");
				}else if(lexInput[index] == '<'){
					index++;
					returnedTokens.emplace_back("<");
				}else if(lexInput[index] == '&'){
					index++;
					returnedTokens.emplace_back("&");
				}else if(lexInput[index] == '|'){
					index++;
					returnedTokens.emplace_back("|");
				}else{
					loopcount++;
					//error

				}
				

			}else if(isalpha(lexInput[index])){
				// check if variable first
				countindex = index;
				while(countindex < lexInput.size() &&
                    (isalpha(lexInput[countindex]) ||
                     isdigit(lexInput[countindex]) ||
                     lexInput[countindex] == '_'   ||
                     lexInput[countindex] == '`' )){
					countindex++;
				}
				if(lexInput[countindex] == '('){
					// if functions call
					returnedTokens.emplace_back(lexInput.substr(index,countindex-index + 1));
					index += countindex-index + 1;
					returnedTokens.emplace_back("(");
				}else{
					returnedTokens.emplace_back(lexInput.substr(index,countindex-index));
					index += countindex-index;
				}
			}else if(isdigit(lexInput[index]) || lexInput[index] == '.'){
				countindex = index;
				decimal:
					while(countindex < lexInput.size() && isxdigit(lexInput[countindex])){
						countindex++;
					}
					if(lexInput[countindex] == '.'){
						countindex++;
						goto decimal;
					}else if(( isxdigit(lexInput[countindex]) ||
					 lexInput[countindex] == '-' ||
					 lexInput[countindex] == '+') &&
					 (lexInput[countindex - 1] == 'e' ||
					  lexInput[countindex - 1] == 'E')){
						countindex++;
						goto decimal;			
					}else if(lexInput[countindex] == 'b'){
						countindex++;
						goto decimal;						
					}else if(lexInput[countindex] == 'o'){
						countindex++;
						goto decimal;						
					}else if(lexInput[countindex] == 'x'){
						countindex++;
						goto decimal;						
					}else if(lexInput[countindex] == 'j'){
						countindex++;
						break;
					}
				returnedTokens.emplace_back(lexInput.substr(index,countindex-index));
				index += countindex-index;
			}else if(lexInput[index] == ','){
				returnedTokens.emplace_back(","); // default sep
				index++;
			}else{
				index++;
			}
			loopcount++;
			if(loopcount >= 2*lexInput.size()){
				break; // Error
			}
		}
		for(unsigned long comb_index = 1; comb_index < returnedTokens.size(); comb_index++){
			if(returnedTokens[comb_index-1] == "==" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "===";
			}else if(returnedTokens[comb_index-1] == "**" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "**=";
			}else if(returnedTokens[comb_index-1] == "<<" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "<<=";
			}else if(returnedTokens[comb_index-1] == ">>" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = ">>=";
			}else if(returnedTokens[comb_index-1] == "+" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "+=";
			}else if(returnedTokens[comb_index-1] == "-" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "-=";
			}else if(returnedTokens[comb_index-1] == "*" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "*=";
			}else if(returnedTokens[comb_index-1] == "/" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "/=";
			}else if(returnedTokens[comb_index-1] == "%" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "%=";
			}else if(returnedTokens[comb_index-1] == "^" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "^=";
			}else if(returnedTokens[comb_index-1] == "=" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "==";
			}else if(returnedTokens[comb_index-1] == "!" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "!=";
			}else if(returnedTokens[comb_index-1] == ">" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = ">=";
			}else if(returnedTokens[comb_index-1] == "<" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "<=";
			}else if(returnedTokens[comb_index-1] == "|" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "|=";
			}else if(returnedTokens[comb_index-1] == "&" && returnedTokens[comb_index] == "="){
				returnedTokens.erase(returnedTokens.begin()+comb_index-1);
				returnedTokens[comb_index-1] = "&=";
			}else{
				// Error
			}
		}
		std::string().swap(lexInput);
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
		output.reserve(tokensInput.size());
		for(unsigned long int index = 0;index<tokensInput.size();index++){
			if(!isdigit(tokensInput[index][0])){
				if(tokensInput[index] == ","){
					token tk("SEP",7);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "+"){
					token tk("ADD",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "-"){
					token tk("SUB",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "*"){
					token tk("MUL",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "/"){
					token tk("DIV",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "%"){
					token tk("MOD",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "^"){
					token tk("BITXOR",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "!"){
					token tk("FACT",6);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "("){
					token tk("L_BRAC",2);
					output.emplace_back(tk);
				}else if(tokensInput[index] == ")"){
					token tk("R_BRAC",3);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "="){
					token tk("ASN",10);
					output.emplace_back(tk); // Assign
				}else if(tokensInput[index] == "**"){
					token tk("POW",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "//"){
					token tk("FLOORDIV",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "||"){
					token tk("LOR",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "&&"){
					token tk("LAND",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "&"){
					token tk("BITAND",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "|"){
					token tk("BITOR",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == ">>"){
					token tk("SHR",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "<<"){
					token tk("SHL",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "==="){
					token tk("TEQL",100); // Special
					output.emplace_back(tk); // Equal
				}else  if(tokensInput[index] == "=="){
					token tk("EQL",1);
					output.emplace_back(tk); // Equal
				}else if(tokensInput[index] == "!="){
					token tk("NQL",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == ">="){
					token tk("GQL",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "<="){
					token tk("LQL",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == ">"){
					token tk("GRT",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "<"){
					token tk("LST",1);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "+="){
					token tk("ADDASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "-="){
					token tk("SUBASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "*="){
					token tk("MULASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "/="){
					token tk("DIVASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "^="){
					token tk("BITXORASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "<<="){
					token tk("SHLASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == ">>="){
					token tk("SHRASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "**="){
					token tk("POWASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "|="){
					token tk("BITORASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index] == "&="){
					token tk("BITANDASN",10);
					output.emplace_back(tk);
				}else if(tokensInput[index][0] == '.'){
					token tk(tokensInput[index],0);
					output.emplace_back(tk);
				}else if(tokensInput[index].back() != '(' && (isalpha(tokensInput[index].front()) || tokensInput[index].front() == '_')){
					token tk(tokensInput[index],5);
					output.emplace_back(tk);
				}else if(tokensInput[index].back() == '('){
					size_t d = std::count(tokensInput[index].begin(), tokensInput[index].end(), '`');
					tokensInput[index].erase(std::remove(tokensInput[index].begin(), tokensInput[index].end(), '`'), tokensInput[index].end());
					token tk(tokensInput[index],4);
					if(index+2 < tokensInput.size() && tokensInput[index+2] == ")"){
						tk.reserved = 0;
					}else{
						tk.reserved = 1;
					}
					if(d == std::string::npos){
						tk.special = 0;
					}else{
						tk.special = static_cast<long int>(d);
					}
					output.emplace_back(tk);
				}else if((tokensInput[index].front() == '"' && tokensInput[index].back() == '"') ||
					(tokensInput[index].front() == '\'' && tokensInput[index].back() == '\'')){
					token tk(tokensInput[index], 9);
					output.emplace_back(tk);
				}else{
					token tk("NULL",-1);
					output.emplace_back(tk);
				}
			}else if(tokensInput[index].back() == '('){
					size_t d = std::count(tokensInput[index].begin(), tokensInput[index].end(), '`');
					tokensInput[index].erase(std::remove(tokensInput[index].begin(), tokensInput[index].end(), '`'), tokensInput[index].end());
					token tk(tokensInput[index],4);
					if(index+2 < tokensInput.size() && tokensInput[index+2] == ")"){
						tk.reserved = 0;
					}else{
						tk.reserved = 1;
					}
					if(d == std::string::npos){
						tk.special = 0;
					}else{
						tk.special = static_cast<long int>(d);
					}
					output.emplace_back(tk);
			}else if(ttype(tokensInput[index]) == 0){
				token tk(tokensInput[index],0);
				output.emplace_back(tk);			
			}else if((tokensInput[index].front() == '"' && tokensInput[index].back() == '"') ||
					(tokensInput[index].front() == '\'' && tokensInput[index].back() == '\'')){
				token tk(tokensInput[index], 9);
				output.emplace_back(tk);
			}else{
				token tk(tokensInput[index],5);
				output.emplace_back(tk);
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
					output.at(replidx - 1).type == 7 ||
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
		std::vector<std::string>().swap(tokensInput);
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

	// Removes all whitespace that 
	std::string removeWhiteSpace(std::string str){
		bool instring = false;
		for(unsigned long int rindx = 0; rindx < str.size(); rindx++){
			if(!instring && isspace(str[rindx])){
				str.erase(str.begin()+rindx);
			}
			if(str[rindx] == '"' || str[rindx] == '\''){
				if(!instring){
					instring = true;
				}else{
					if(rindx >= 1){
						if(str[rindx - 1] == '\\') continue;
					}else{
						instring = false;
					}
				}
			}
		}
		return str;
	}

	std::string stripcomment(std::string str){
		bool take = true;
		std::string out;
		out.reserve(str.size());
		for(unsigned long int idx = 0; idx < str.size(); idx++){
			if(idx+1 < str.size()){
				if(str[idx] == '/' && str[idx + 1] == '/'){
					return str.substr(0, idx);
				}
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
			if(ieqpos + 2 < (long int)str.size()){
				if(str[ieqpos + 1] == '=' && str[ieqpos + 2] == '='){
					return 1; // CAS
				}
			}
			if(ieqpos - 2 >= 0){
				if(str[ieqpos - 2] == '>' || str[ieqpos - 2] == '<' || str[ieqpos - 2] == '*'){
					return 0;
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
					return 0;
				}else{
					long int fbracpos = static_cast<long int>(fbrac);
					if(fbracpos < ieqpos){
						return 2;
					}else{
						return 0;
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