#ifndef ZETACOMPILER_HPP
#define ZETACOMPILER_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Variable.hpp"

namespace comp {
	bool string_isnum(std::string str);

	int ttype(std::string var);

	unsigned int precedence(std::string op);

	bool associativity(std::string op);

	std::vector<std::string> lex(std::string lexInput);

	std::vector<std::string> tokenComp(std::vector<std::string> tokensInput);

	int checkBrac(std::vector<std::string> tokens);
	
	std::vector<std::string> shuntingYard(std::vector<std::string> tokens);	
}

#endif