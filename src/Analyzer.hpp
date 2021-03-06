/*  Syntax analysis header.
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

#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include<string>
#include<vector>

#include "ZetaStack.hpp"

bool checkrpn(const std::vector<token>& checktokens);

bool squotecount(const std::string& str);

bool quotecount(const std::string& str);

int checkleftBrac(const std::string& str);

int checkrightBrac(const std::string& str);

bool bracketOrder(const std::vector<token>& tokens);

#endif