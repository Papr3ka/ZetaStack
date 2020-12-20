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
			return 10;
		}else if(op == "NEG" || op == "POS"){
			return 9;
		}else if(op == "POW"){
			return 8;
		}else if(op == "MUL" || op == "DIV" || op == "MOD" || op == "FLOORDIV"){
			return 7;
		}else if(op == "ADD" || op == "SUB"){
			return 6;
		}else if(op == "SHL" || op == "SHR"){
			return 5;
		}else if(op == "AND"){
			return 4;
		}else if(op == "XOR"){
			return 3;
		}else if(op == "OR"){
			return 2;
		}else if(op == "EQL" || op == "NQL" || op == "GQL" || op == "LQL"){
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
		   op == "LQL"){
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

	// fcomp = false, bool used for function jit comp
	std::vector<token> shuntingYard(std::vector<token> tokens){
		std::vector<token> operatorStack;
		std::vector<token> outputQueue;
		std::vector<token> functionStack;
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
			*/
			switch(tokens.front().type){
				case 0: // NUM
					outputQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 1: // OPERATOR
					while(!operatorStack.empty()){
						if(((precedence(operatorStack.back().data) > precedence(tokens.front().data) ||
						((precedence(operatorStack.back().data) == precedence(tokens.front().data)) && associativity(tokens.front().data)))) &&
						operatorStack.back().data != "L_BRAC"){							
							outputQueue.push_back(operatorStack.back());
							operatorStack.pop_back();
						}else{
							break;
						}
					}
					operatorStack.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 2:
					layer++;
					operatorStack.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 3:
					while(!operatorStack.empty() && (operatorStack.back().data != "L_BRAC")){
						outputQueue.push_back(operatorStack.back());
						operatorStack.pop_back();
					}
					if(!operatorStack.empty() && operatorStack.back().data == "L_BRAC"){
						operatorStack.pop_back();
					}
					tokens.erase(tokens.begin());
					layer--;
					if(layer == infunction && !functionStack.empty()){					
						outputQueue.push_back(functionStack.back());
						functionStack.pop_back();
						infunction--;
					}		
					break;
				case 4:
					infunction++;
					layer = infunction;
					functionStack.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 5: // Variable
					outputQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 6: // Left Function
					outputQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 7: // SEP - Dump all
					tokens.erase(tokens.begin());
					while(!operatorStack.empty()){
						if(operatorStack.back().data == "L_BRAC" || operatorStack.back().data == "R_BRAC"){
							operatorStack.pop_back();
						}
						if(operatorStack.back().type == 4){
							functionStack.push_back(operatorStack.back());
							operatorStack.pop_back();
						}else if(operatorStack.size() >= 1){
							outputQueue.push_back(operatorStack.back());
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
			outputQueue.push_back(operatorStack.back());
			operatorStack.pop_back();
		}
		while(!functionStack.empty()){
			outputQueue.push_back(functionStack.back());
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
				token tk;
				tokens.at(index).data = var::search(tokens.at(index).data);
				tokens.at(index).type = 0;
			}
		}
		return tokens;
	}

}