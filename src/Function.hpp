/* Header for Function.cpp
 * 
 * Copyright (c) 2020-2021 Benjamin Yao.
 * 
 * This file is part of ZetaStack.
 * 
 * ZetaStack is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * ZetaStack is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include<string>
#include<unordered_map>
#include<vector>

#include "ZetaStack.hpp"

extern bool safe_mode;
extern unsigned long int maxobj;

struct default_modifier{

    bool initialized = false;
    token asn_type;
    token rvalue;

    default_modifier():
    initialized(false)
    {}

    default_modifier(token asn_type, token rvalue):
    initialized(true), asn_type(asn_type), rvalue(rvalue)
    {}
};

struct mod_map{
    unsigned long int order;
    default_modifier df;

    mod_map(){}
};

token lookup(token var, std::vector<token> identifiers, std::vector<token> args);

std::vector<token> fillvars(const std::vector<token>& argsname, const std::vector<token>& argsvar, const std::vector<token>& fbody);

extern unsigned long int filled_core;

extern unsigned long int filled_builtin;

void initcore(bool safe);

void initbuiltin(bool safe);

void free_core(void);

void free_builtin(void);

bool returns(std::string fname, long int argcount);

double callspecial(std::vector<token> fargs, std::string name);

bool f_isspecial(std::string name, long int argcounts);

bool f_isnamespecial(std::string name);

bool f_iscorefunc(std::string name, long int argcounts);

bool f_isnamecorefunc(std::string name);

token callcore(std::vector<token> fargs, std::string name);

void delAll(void);

void def(std::string name, std::vector<token> assignTo, std::vector<token> body, std::unordered_map<std::string, default_modifier> defaults);

int udef(std::string name, long int argcounts = -1);

bool fexists(std::string name, signed long int argcounts);

std::vector<token> call(std::vector<token> fargs, std::string name);

std::vector<token> deriv(std::vector<token> funcargs, std::string name, long int d, double hval, unsigned long int evalx);

long int argcount(std::string name);

unsigned long int funcsCount(void);

std::vector<std::string> getandassemble_all_defined_functions(void);

std::vector<std::string> getandassemble_all_builtin_functions(void);

#endif