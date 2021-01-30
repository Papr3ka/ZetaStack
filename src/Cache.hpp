/* Header for Cache.cpp
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

#ifndef CACHE_HPP
#define CACHE_HPP

#include<string>
#include<vector>


#include "ZetaStack.hpp"

// Function Prototypes

struct ansctable{
	std::string key;
	std::string ans;

	ansctable(const std::string& k, const std::string& a):
	key(k), ans(a)
	{}
};

namespace cch {

	void setmaxlen(unsigned long int x);

	void addentry(std::string key, std::string ans);

	std::string searchentry(std::string key);

	std::vector<ansctable> getentries(void);

	void clearcache(void);

}

#endif