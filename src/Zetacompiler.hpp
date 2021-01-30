/* Header for Zetacompiler.cpp
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

#ifndef ZETACOMPILER_HPP
#define ZETACOMPILER_HPP

#include<string>
#include<unordered_map>
#include<vector>

#include "Variable.hpp"
#include "ZetaStack.hpp"

struct comp_metadata{
    unsigned long int nums;
    unsigned long int operators;
    unsigned long int bracs;
    unsigned long int functions;

    comp_metadata() = default;
};

template<class T>
struct splittedvec{
    std::vector<T> first;
    std::vector<T> second;

    splittedvec() = default;
};

// Function Prototypes

namespace comp {

    token getop(unsigned long idx);

    bool string_isnum(std::string str);

    int ttype(std::string var);

    bool hasvar(std::vector<token> tokens);

    comp_metadata getcompdata(std::vector<token> vecdata);
    
    std::vector<token> shuntingYard(std::vector<token> tokens, comp_metadata meta);

    void fillallvars(std::vector<token>& tokens);

    std::vector<token> retfillallvars(std::vector<token> tokens);

    std::vector<token> retfillspecificvars(std::vector<token> tokens, std::unordered_map<std::string, token> tofill);

    splittedvec<token> splitvectorPoint(std::vector<token> vect, unsigned long int point);

    std::string unmangle(token tk);
}

#endif