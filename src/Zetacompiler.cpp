#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include<iostream>

#include "Zetacompiler.hpp"
#include "Function.hpp"
#include "Variable.hpp"
#include "Token.hpp"

namespace comp {

	std::vector<std::string> assign{
		"ASN",
		"ADDASN",
		"SUBASN",
		"MULASN",
		"DIVASN",
		"MODASN",
		"XORASN",
		"POWASN",
		"SHLASN",
		"SHRASN"
	};

	std::vector<std::string> operators{
		"ADD",
		"SUB",
		"MUL",
		"DIV",
		"MOD",
		"XOR",
		"POW",
		"SHL",
		"SHR",
		"FLOORDIV",
		"OR",
		"AND",
		"EQL",
		"NQL",
		"GQL",
		"LQL",
		"GRT",
		"LST"
	};

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

	int ttype(std::string var){
		if(var == "SEP"){
			return 7;
		}else if(var == "L_BRAC"){
			return 2;
		}else if(var == "R_BRAC"){
			return 3;
		}else if(string_isnum(var)){
			return 0;
		}else if(isalpha(var[0])){
			bool isop = false;
			for(unsigned long int inx=0; inx < operators.size(); inx++){
				if(var == operators[inx]){
					isop = true;
				}
			}
			if(isop){
				return 1;
			}else if(var == "FACT"){
				return 6;
			}else if(var.back() == '('){
					return 4;
			}
		}
		return 5;
	}
		/*
			0 - NUM
			1 - OPERATOR
			2 - LEFT BRACKET
			3 - RIGHT BRACKET
			4 - FUNCTION
			5 - VARIABLE
			6 - R FUNC
			7 - Separator
			10 - Assigning operator
		*/


	token getop(unsigned long idx){
		token tk(operators.at(idx), 1);
		return tk;
	}

	bool hasvar(std::vector<token> tokens){
		for(unsigned long int index = 0; index < tokens.size(); index++){
			if(tokens[index].type == 5){
				return true;
			}
		}
		return false;
	}	

	inline unsigned int precedence(std::string op){
		if(op.back() == '('){
			return 20;
		}else if(op == "NEG" || op == "POS"){
			return 19;
		}else if(op == "POW"){
			return 18;
		}else if(op == "MUL" || op == "DIV" || op == "MOD" || op == "FLOORDIV"){
			return 17;
		}else if(op == "ADD" || op == "SUB"){
			return 16;
		}else if(op == "SHL" || op == "SHR"){
			return 15;
		}else if(op == "GQL" || op == "LQL" || op == "GRT" || op == "LST"){
			return 14;
		}else if(op == "EQL" || op == "NQL"){
			return 13;
		}else if(op == "BITAND"){
			return 12;
		}else if(op == "BITXOR"){
			return 11;
		}else if(op == "BITOR"){
			return 10;
		}else if(op == "LAND"){
			return 3;
		}else if(op == "LOR"){
			return 2;
		}else if(op.substr(op.size()-3,op.size()) == "ASN"){
			return 1;
		}else{
			return 0; // ERROR
		}
	}

	// true = LR, false = RL
	inline bool associativity(std::string op){
		if(op == "MUL" || 
		   op == "DIV" || 
		   op == "MOD" || 
		   op == "FLOORDIV" ||
		   op == "ADD" ||
		   op == "SUB" ||
		   op == "SHL" ||
		   op == "SHR" ||
		   op == "AND" ||
		   op == "XOR" ||
		   op == "OR"  ||
		   op == "EQL" ||
		   op == "NQL" ||
		   op == "GQL" ||
		   op == "LQL" ||
		   op == "GRT" ||
		   op == "LST"){
			return true;
		}else{
			return false;
		}
	}

	// Error Checking
	int checkleftBrac(std::string str){
		int count = 0;
		bool instring = false;
		for(unsigned long int idx = 0; idx < str.size(); idx++){
			if(str[idx] == '"'){
				if(idx > 0 && str[idx-1] != '\\'){
					instring = !instring;
				}else if(idx == 0){
					instring = !instring;
				}
			}
			if(str[idx] == '(' && !instring) count++;
		}
		return count;
	}

	int checkrightBrac(std::string str){
		int count = 0;
		bool instring = false;
		for(unsigned long int idx = 0; idx < str.size(); idx++){
			if(str[idx] == '"'){
				if(idx > 0 && str[idx-1] != '\\'){
					instring = !instring;
				}else if(idx == 0){
					instring = !instring;
				}
			}
			if(str[idx] == ')') count++;
		}
		return count;
	}

	comp_metadata getcompdata(std::vector<token> vecdata){
		comp_metadata output{0,0,0,0};
		for(token tmp: vecdata){
			switch(tmp.type){
				case 0:
					output.nums++;
					break;
				case 10:
				case 6:
				case 1:
					output.operators++;
					break;
				case 2:
				case 3:
					output.bracs++;
					break;
				case 4:
					output.functions++;
					break;
			}
		}
		return output;
	}

	std::vector<token> shuntingYard(std::vector<token> tokens, comp_metadata meta){
		// Main vectors
		std::vector<token> operatorStack;
		std::vector<token> outputQueue;
		std::vector<token> functionStack;

		// Vectors for function use
		std::vector<unsigned long int> lockstack;
		std::vector<signed long int> argumentcounter;

		//Reserve vectors so you dont have to keep resizing (increase speed and efficiency)
		operatorStack.reserve(meta.operators+meta.bracs);
		outputQueue.reserve(meta.nums+meta.operators+meta.functions);
		functionStack.reserve(meta.functions);

		lockstack.reserve(meta.functions);
		argumentcounter.reserve(meta.functions);

		// function use
		long int layer = 0;
		long int infunction = 0;

		// Token to be read
		unsigned long int readindex = 0;
		unsigned long int readsize = tokens.size();
		
		// Debug code
		// for(token x: tokens){
		// 	std::cout << " " << x.type << "\n";
		// }

		while(readindex < readsize){
			/*
			0 - NUM
			1 - OPERATOR
			2 - LEFT BRACKET
			3 - RIGHT BRACKET
			4 - FUNCTION
			5 - VARIABLE
			6 - R FUNC
			7 - SEP

			10 - Assignment operator
			*/
			switch(tokens.at(readindex).type){
				case 0: // NUM
					outputQueue.emplace_back(tokens.at(readindex));
					readindex++;
					break;
				case 1: // OPERATOR
				case 10: // Assignment operator
					while(!operatorStack.empty()){
						if(((precedence(operatorStack.back().data) > precedence(tokens.at(readindex).data) ||
						((precedence(operatorStack.back().data) == precedence(tokens.at(readindex).data)) && associativity(tokens.at(readindex).data)))) &&
						operatorStack.back().data != "L_BRAC"){							
							outputQueue.emplace_back(operatorStack.back());
							operatorStack.pop_back();
						}else{
							break;
						}
					}
					operatorStack.emplace_back(tokens.at(readindex));
					readindex++;
					break;
				case 2:
					layer++;
					operatorStack.emplace_back(tokens.at(readindex));
					readindex++;
					break;
				case 3:
					while(!operatorStack.empty() && (operatorStack.back().data != "L_BRAC")){
						outputQueue.emplace_back(operatorStack.back());
						operatorStack.pop_back();
					}
					if(!operatorStack.empty() && operatorStack.back().data == "L_BRAC"){
						operatorStack.pop_back();
					}
					readindex++;
					layer--;
					if(layer == infunction && !functionStack.empty()){
						functionStack.back().reserved = argumentcounter.back();							
						outputQueue.emplace_back(functionStack.back());
						functionStack.pop_back();
						lockstack.pop_back();
						argumentcounter.pop_back();
						infunction--;
					}		
					break;
				case 4:
					infunction++;
					layer = infunction;
					functionStack.emplace_back(tokens.at(readindex));
					argumentcounter.emplace_back(tokens.at(readindex).reserved);
					readindex++;
					lockstack.emplace_back(operatorStack.size() + 1);
					break;
				case 5: // Variable
					outputQueue.emplace_back(tokens.at(readindex));
					readindex++;
					break;
				case 6: // Left Function
					outputQueue.emplace_back(tokens.at(readindex));
					readindex++;
					break;
				case 7: // SEP - Dump all
					if(!argumentcounter.empty()) argumentcounter.back()++;
					readindex++;
					while(!operatorStack.empty() && operatorStack.size() > lockstack.back()){
						if(operatorStack.back().data == "L_BRAC" || operatorStack.back().data == "R_BRAC"){
							operatorStack.pop_back();
						}
						if(operatorStack.back().type == 4){
							functionStack.emplace_back(operatorStack.back());
							operatorStack.pop_back();
						}else if(operatorStack.size() >= 1){
							outputQueue.emplace_back(operatorStack.back());
							operatorStack.pop_back();
						}
					}
					break;
				default:
					outputQueue.emplace_back(tokens.at(readindex));
					readindex++;
					break;
			}
		}
		while(!operatorStack.empty()){
			if(operatorStack.back().data == "L_BRAC" || operatorStack.back().data == "R_BRAC"){
				operatorStack.pop_back();
			}
			outputQueue.emplace_back(operatorStack.back());
			operatorStack.pop_back();
		}
		for(unsigned long int store=0;store < argumentcounter.size(); store++){
			functionStack.at(store).reserved = argumentcounter.at(store);
		}
		if(!functionStack.empty()){
			std::copy(functionStack.begin(), functionStack.end(),outputQueue.end());
		}		

		return outputQueue;
	}

	// tokens = compiled list of tokens, to be used at last step after recurselink
	std::vector<token> fillallvars(std::vector<token> tokens){
		for(unsigned long int index=0;index < tokens.size(); index++){
			if(tokens.at(index).type == 5){
				if(index+1 < tokens.size()){
					if(tokens.at(index+1).type == 10){
						continue;
					}
				}
				token tk(var::search(tokens.at(index).data, false), 0);
				tokens.at(index) = tk;
			}
		}
		return tokens;
	}

}