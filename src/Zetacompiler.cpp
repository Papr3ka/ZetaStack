#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include<iostream>

#include "Zetacompiler.hpp"
#include "Variable.hpp"

namespace comp {

	std::vector<std::string> functions{
		"sin(",
		"cos(",
		"tan("
	}; // len 3

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
		if(var == "L_BRAC"){
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

	// Lexical Analysis
	std::vector<std::string> lex(std::string lexInput){
		std::vector<std::string> returnedTokens;
		std::string dualchar;
		unsigned long int loopcount = 0;
		unsigned long int index = 0;
		while(index <= lexInput.size()){
			if(ispunct(lexInput[index]) && lexInput[index] != '.'){
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
				if(countindex >= lexInput.size()){
					returnedTokens.push_back(lexInput.substr(index,countindex-index));
					index += countindex-index;
				}else if(lexInput[countindex] == '('){
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
			}else{
				index++;
			}
			loopcount++;
			if(loopcount >= 2*lexInput.size()){
				break;
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

	std::vector<std::string> tokenComp(std::vector<std::string> tokensInput){
		std::vector<std::string> output;
		for(unsigned long int index = 0;index<tokensInput.size();index++){
			if(!isdigit(tokensInput[index][0])){
				if(tokensInput[index] == "+"){
					output.push_back("ADD");
				}else if(tokensInput[index] == "-"){
					output.push_back("SUB");
				}else if(tokensInput[index] == "*"){
					output.push_back("MUL");
				}else if(tokensInput[index] == "/"){
					output.push_back("DIV");
				}else if(tokensInput[index] == "%"){
					output.push_back("MOD");
				}else if(tokensInput[index] == "^"){
					output.push_back("XOR");
				}else if(tokensInput[index] == "!"){
					output.push_back("FACT");
				}else if(tokensInput[index] == "("){
					output.push_back("L_BRAC");
				}else if(tokensInput[index] == ")"){
					output.push_back("R_BRAC");
				}else if(tokensInput[index] == "="){
					output.push_back("ASN"); // Assign
				}else if(tokensInput[index] == "**"){
					output.push_back("POW");
				}else if(tokensInput[index] == "//"){
					output.push_back("FLOORDIV");
				}else if(tokensInput[index] == "||"){
					output.push_back("OR");
				}else if(tokensInput[index] == "&&"){
					output.push_back("AND");
				}else if(tokensInput[index] == "<<"){
					output.push_back("SHL");
				}else if(tokensInput[index] == ">>"){
					output.push_back("SHR");
				}else if(tokensInput[index] == "=="){
					output.push_back("EQL"); // Equal
				}else if(tokensInput[index] == "!="){
					output.push_back("NQL");
				}else if(tokensInput[index] == ">="){
					output.push_back("GQL");
				}else if(tokensInput[index] == "<="){
					output.push_back("LQL");
				}else if(tokensInput[index] == "+="){
					output.push_back("ADDASN");
				}else if(tokensInput[index] == "-="){
					output.push_back("SUBASN");
				}else if(tokensInput[index] == "*="){
					output.push_back("MULASN");
				}else if(tokensInput[index] == "/="){
					output.push_back("DIVASN");
				}else if(tokensInput[index] == "^="){
					output.push_back("XORASN");
				}else if(tokensInput[index] == "<<="){
					output.push_back("SHLASN");
				}else if(tokensInput[index] == ">>="){
					output.push_back("SHRASN");
				}else if(tokensInput[index] == "**="){
					output.push_back("POWASN");
				}else if(tokensInput[index] == "//="){
					output.push_back("FLOORDIVASN");
				}else if(tokensInput[index][0] == '.'){
				output.push_back(tokensInput[index]);
				}else if(isalpha(tokensInput[index][0])){
					output.push_back(tokensInput[index]);
				}else{
					output.push_back("NULL");
				}
			}else{
				output.push_back(tokensInput[index]);
			}
		}
		if(output.size() > 1){
			if(output[0] == "SUB"){			
				output.erase(output.begin());
				output[0].insert(output[0].begin(),1,'-');
				//std::cout << output[0] << "\n";
			}
		}
		// Unary Sign
		for(unsigned long int idx=1; idx < output.size() - 1; idx++){
			if(ttype(output[idx - 1]) == 1 && output[idx] == "SUB" && ttype(output[idx + 1]) == 0){
				output.erase(output.begin() + idx);
				if(output[idx].front() == '+' || output[idx].front() == '-'){
					output[idx][0] = '-';
				}else{
					output[idx].insert(output[idx].begin(),1,'-');
				}
			}else if(ttype(output[idx - 1]) == 1 && output[idx] == "ADD" && ttype(output[idx + 1]) == 0){
				output.erase(output.begin() + idx);
				if(output[idx].front() == '+' || output[idx].front() == '-'){
					output[idx][0] = '+';
				}else{
					output[idx].insert(output[idx].begin(),1,'+');
				}
			}else{
				// Error
			}
		}
		return output;
	}

	// Error Checking
	int checkBrac(std::vector<std::string> tokens){
		std::vector<int> outErr;
		int lbrac_count = std::count(tokens.begin(),tokens.end(), "(");
		if(std::count(tokens.begin(),tokens.end(), "L_BRAC") > lbrac_count){
			lbrac_count = std::count(tokens.begin(),tokens.end(), "L_BRAC");
		}
		int rbrac_count = std::count(tokens.begin(),tokens.end(), ")");
		if(std::count(tokens.begin(),tokens.end(), "R_BRAC") > rbrac_count){
			rbrac_count = std::count(tokens.begin(),tokens.end(), "R_BRAC");
		}
		return abs(lbrac_count - rbrac_count);
	}

	std::vector<std::string> shuntingYard(std::vector<std::string> tokens){
		std::vector<std::string> operatorStack;
		std::vector<std::string> outputQueue;
		std::string vars;
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
					operatorStack.push_back(tokens.front());
					tokens.erase(tokens.begin());
					break;
				case 5: // Variable
					vars = var::search(tokens.front());
					if(vars != "NULL"){
						outputQueue.push_back(vars);
					}else{
						outputQueue.push_back("NULL");
					}
					tokens.erase(tokens.begin());

					break;
				case 6: // Left Function
					outputQueue.push_back(tokens.front());
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
	// Wrapper functions
	extern void updatevar(std::string iden, std::string val){
		var::update(iden, val);
	}

	extern std::string searchvar(std::string iden){
		return var::search(iden);
	}

	extern int delvar(std::string variden){
		return var::delvar(variden);
	}

}