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

	unsigned int precedence(std::string op){
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
	bool associativity(std::string op){
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
		for(unsigned long int idx = 0; idx < str.size(); idx++){
			if(str[idx] == '(') count++;
		}
		return count;
	}
	int checkrightBrac(std::string str){
		int count = 0;
		for(unsigned long int idx = 0; idx < str.size(); idx++){
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

	// fcomp = false, bool used for function jit comp
	std::vector<token> shuntingYard(std::vector<token> tokens, comp_metadata meta){
		std::vector<token> operatorStack;
		std::vector<token> outputQueue;
		std::vector<token> functionStack;
		operatorStack.reserve(meta.operators+meta.bracs);
		outputQueue.reserve(meta.nums+meta.operators+meta.functions);
		outputQueue.reserve(meta.functions);
		std::vector<unsigned long int> lockstack;
		lockstack.reserve(meta.functions);
		long int layer = 0;
		long int infunction = 0;
		
		// Debug code
		// for(token x: tokens){
		// 	std::cout << " " << x.type << "\n";
		// }

		while(!tokens.empty()){
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
			switch(tokens.front().type){
				case 0: // NUM
					outputQueue.emplace_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 1: // OPERATOR
				case 10: // Assignment operator
					while(!operatorStack.empty()){
						if(((precedence(operatorStack.back().data) > precedence(tokens.front().data) ||
						((precedence(operatorStack.back().data) == precedence(tokens.front().data)) && associativity(tokens.front().data)))) &&
						operatorStack.back().data != "L_BRAC"){							
							outputQueue.emplace_back(operatorStack.back());
							operatorStack.pop_back();
						}else{
							break;
						}
					}
					operatorStack.emplace_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 2:
					layer++;
					operatorStack.emplace_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 3:
					while(!operatorStack.empty() && (operatorStack.back().data != "L_BRAC")){
						outputQueue.emplace_back(operatorStack.back());
						operatorStack.pop_back();
					}
					if(!operatorStack.empty() && operatorStack.back().data == "L_BRAC"){
						operatorStack.pop_back();
					}
					tokens.erase(tokens.begin());
					layer--;
					if(layer == infunction && !functionStack.empty()){					
						outputQueue.emplace_back(functionStack.back());
						functionStack.pop_back();
						lockstack.pop_back();
						infunction--;
					}		
					break;
				case 4:
					infunction++;
					layer = infunction;
					functionStack.emplace_back(tokens.front());
					tokens.erase(tokens.begin());
					lockstack.emplace_back(operatorStack.size() + 1);
					break;
				case 5: // Variable
					outputQueue.emplace_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 6: // Left Function
					outputQueue.emplace_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 7: // SEP - Dump all
					tokens.erase(tokens.begin());
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
					tokens.erase(tokens.begin());
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
		while(!functionStack.empty()){
			outputQueue.emplace_back(functionStack.back());
			functionStack.pop_back();
		}
		unsigned long int checkidx = 0;
		while(checkidx < outputQueue.size()){
			if(outputQueue[checkidx].type == 2){
				outputQueue.erase(outputQueue.begin() + checkidx);
			}else{
				checkidx++;
			}
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
				tokens.at(index).data = var::search(tokens.at(index).data);
				tokens.at(index).type = 0;
			}
		}
		return tokens;
	}

}