#ifndef ZETACOMPILER_HPP
#define ZETACOMPILER_HPP

#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<string>
#include<vector>

#include "Token.hpp"
#include "Variable.hpp"

typedef struct{
	unsigned long int nums;
	unsigned long int operators;
	unsigned long int bracs;
	unsigned long int functions;
} comp_metadata;

// Function Prototypes

namespace comp {

	token getop(unsigned long idx);

	bool string_isnum(std::string str);

	int ttype(std::string var);

	bool hasvar(std::vector<token> tokens);

	comp_metadata getcompdata(std::vector<token> vecdata);
	
	std::vector<token> shuntingYard(std::vector<token> tokens, comp_metadata meta);

	std::vector<token> fillallvars(std::vector<token> tokens);
}

#endif