#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<string>
#include<vector>

#include "Function.hpp"
#include "Token.hpp"
#include "Zetacompiler.hpp"

token lookup(token var, std::vector<token> identifiers, std::vector<token> args);

std::vector<token> fillvars(std::vector<token> argsname, std::vector<token> argsvar, std::vector<token> fbody);

void def(std::vector<token> assignTo, std::vector<token> body);

int udef(std::string name, long int argcounts = -1);

bool fexists(std::string name, signed long int argcounts);

std::vector<token> call(std::vector<token> fargs, std::string name);

long int argcount(std::string name);

#endif