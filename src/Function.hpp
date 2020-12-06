#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Function.hpp"
#include "Zetacompiler.hpp"
#include "Token.hpp"

void def(std::vector<token> assignTo, std::vector<token> body);

void udef(std::string name);

std::vector<token> call(std::vector< std::vector<token> > fargs, std::string name);

unsigned long int argcount(std::string name);

void clearfuncdata(std::string name);

#endif