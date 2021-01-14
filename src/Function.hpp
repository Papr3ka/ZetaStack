#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<string>
#include<vector>

#include "Token.hpp"

extern bool safe_mode;
extern unsigned long int maxobj;

token lookup(token var, std::vector<token> identifiers, std::vector<token> args);

std::vector<token> fillvars(std::vector<token> argsname, std::vector<token> argsvar, std::vector<token> fbody);

void initcore(bool safe);

void initbuiltin(bool safe);

double callspecial(std::vector<token> fargs, std::string name);

bool f_isspecial(std::string name, long int argcounts);

bool f_isnamespecial(std::string name);

bool f_iscorefunc(std::string name, long int argcounts);

bool f_isnamecorefunc(std::string name);

token callcore(std::vector<token> fargs, std::string name);

void def(std::vector<token> assignTo, std::vector<token> body);

int udef(std::string name, long int argcounts = -1);

bool fexists(std::string name, signed long int argcounts);

std::vector<token> call(std::vector<token> fargs, std::string name);

std::vector<token> deriv(std::vector<token> funcargs, std::string name, long int d, double hval, unsigned long int evalx);

long int argcount(std::string name);

unsigned long int funcsCount(void);

std::vector<std::string> getandassemble_all_defined_functions(void);

std::vector<std::string> getandassemble_all_builtin_functions(void);

#endif