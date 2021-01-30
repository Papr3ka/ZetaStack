/* header for Preprocessor.cpp
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

#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include<string>
#include<vector>

#include "ZetaStack.hpp"

template<class T>
struct dipart{
	T front;
	T back;

	dipart() = default;
};

// Function Prototypes

std::vector<std::string> split(std::string str, std::string split);

namespace comp {

	std::vector<std::string> lex(std::string lexInput);

	std::vector<token> tokenComp(std::vector<std::string> tokensInput);

	int assigntype(std::string str);
	
	std::string removeWhiteSpace(std::string str);

	std::string stripcomment(std::string str);

	int execType(std::string str);

	dipart< std::vector<token> > spliteq(std::vector<token> str);

	std::string strchar_parse(const std::string& str);
}

#endif
