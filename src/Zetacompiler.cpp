#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include<iostream>

#include "Zetacompiler.hpp"
#include "Variable.hpp"


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
		"AND"
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


	std::string getop(unsigned long idx){
		return operators.at(idx);
	}

	bool hasvar(std::vector<std::string> tokens){
		for(unsigned long int index = 0; index < tokens.size(); index++){
			if(comp::ttype(tokens[index]) == 5){
				return true;
			}
		}
		return false;
	}	

	unsigned int precedence(std::string op){
		if(op.back() == '('){
			return 8;
		}else if(op == "POW"){
			return 7;
		}else if(op == "MUL" || op == "DIV" || op == "MOD" || op == "FLOORDIV"){
			return 6;
		}else if(op == "ADD" || op == "SUB"){
			return 5;
		}else if(op == "SHL" || op == "SHR"){
			return 4;
		}else if(op == "AND"){
			return 3;
		}else if(op == "XOR"){
			return 2;
		}else if(op == "OR"){
			return 1;
		}else {
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
		   op == "OR"){
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
	std::vector<std::string> shuntingYard(std::vector<std::string> tokens, const bool fcomp){
		std::vector<std::string> operatorStack;
		std::vector<std::string> outputQueue;
		std::vector<std::string> functionQueue;
		std::string vars;
		long int bcount;
		while(!tokens.empty()){
			/*
			0 - NUM
			1 - OPERATOR
			2 - LEFT BRACKET
			3 - RIGHT BRACKET
			4 - FUNCTION
			5 - VARIABLE
			6 - R FUNC
			*/
			switch(ttype(tokens.front())){
				case 0: // NUM
					outputQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 1: // OPERATOR
					while(!operatorStack.empty()){
						if(((precedence(operatorStack.back()) > precedence(tokens.front()) ||
						((precedence(operatorStack.back()) == precedence(tokens.front())) && associativity(tokens.front())))) &&
						operatorStack.back() != "L_BRAC"){								
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
					operatorStack.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 3:
					while(!operatorStack.empty() && (operatorStack.back() != "L_BRAC")){
						outputQueue.push_back(operatorStack.back());
						operatorStack.pop_back();
					}
					if(!operatorStack.empty() && operatorStack.back() == "L_BRAC"){
						operatorStack.pop_back();
					}
					tokens.erase(tokens.begin());
					break;
				case 4: // FUNC
					bcount = 1;
					functionQueue.push_back(tokens.front());
					functionQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					tokens.erase(tokens.begin());
					while(bcount != 0 && !tokens.empty()){
						functionQueue.insert(functionQueue.begin(),tokens.front());
						tokens.erase(tokens.begin());						
						if(ttype(tokens.front()) == 2){
							functionQueue.push_back("L_BRAC");
							bcount++;
						}else if(ttype(tokens.front()) == 3){
							functionQueue.push_back("R_BRAC");
							bcount--;
						}
						
					}
					outputQueue.insert(outputQueue.end() - 1,functionQueue.begin(),functionQueue.end());
					functionQueue.clear();
					break;
				case 5: // Variable
					outputQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 6: // Left Function
					outputQueue.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 7: // SEP
					if(fcomp){
						outputQueue.push_back("SEP");
					}
					tokens.erase(tokens.begin());
					break;
				default:
					tokens.erase(tokens.begin());
					break;
			}
		}
		while(!operatorStack.empty()){
			if(operatorStack.back() == "L_BRAC" || operatorStack.back() == "R_BRAC"){
				operatorStack.pop_back();
			}
			outputQueue.push_back(operatorStack.back());
			operatorStack.pop_back();
		}
		return outputQueue;
	}

	// tokens = compiled list of tokens, to be used at last step after recurselink
	std::vector<std::string> fillallvars(std::vector<std::string> tokens){
		for(unsigned long int index=0;index < tokens.size(); index++){
			if(ttype(tokens.at(index)) == 5){
				tokens.at(index) = var::search(tokens.at(index));
			}
		}
		return tokens;
	}

}