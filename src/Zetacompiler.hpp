#ifndef ZETACOMPILER_HPP
#define ZETACOMPILER_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Variable.hpp"
#include "Token.hpp"

// Function Prototypes

namespace comp {

	token getop(unsigned long idx);

	bool string_isnum(std::string str);

	int ttype(std::string var);

	bool hasvar(std::vector<token> tokens);

	unsigned int precedence(std::string op);

	bool associativity(std::string op);

	std::vector<std::string> lex(std::string lexInput);

	std::vector<token> tokenComp(std::vector<std::string> tokensInput);

	int checkleftBrac(std::string tokens);
	
	int checkrightBrac(std::string tokens);
	
	std::vector<token> shuntingYard(std::vector<token> tokens, const bool fcomp=false);

	std::vector<token> fillallvars(std::vector<token> tokens);
}

#endif