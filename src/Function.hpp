#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

#include "Function.hpp"
#include "Zetacompiler.hpp"

void def(std::vector<std::string> assignTo, std::vector<std::string> body);

void udef(std::string name);

std::vector<std::string> call(std::vector<std::string> fargs);

unsigned long int argcount(std::string name);

void clearfuncdata(std::string name);

#endif