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

token lookup(token var, std::vector<token> identifiers, std::vector<token> args);

std::vector<token> fillvars(std::vector<token> argsname, std::vector<token> argsvar, std::vector<token> fbody);

void def(std::vector<token> assignTo, std::vector<token> body);

int udef(std::string name);

bool fexists(std::string name);

std::vector<token> call(std::vector<token> fargs, std::string name);

long int argcount(std::string name);

#endif