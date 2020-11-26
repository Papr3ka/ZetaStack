#ifndef ZETACOMPILER_HPP
#define ZETACOMPILER_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Variable.hpp"

// Function Prototypes

namespace comp {

	std::string getop(unsigned long idx);

	bool string_isnum(std::string str);

	int ttype(std::string var);

	bool hasvar(std::vector<std::string> tokens);

	unsigned int precedence(std::string op);

	bool associativity(std::string op);

	std::vector<std::string> lex(std::string lexInput);

	std::vector<std::string> tokenComp(std::vector<std::string> tokensInput);

	int checkBrac(std::vector<std::string> tokens);
	
	std::vector<std::string> shuntingYard(std::vector<std::string> tokens);
}

#endif