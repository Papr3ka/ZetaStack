/* Header for Variable.cpp
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

#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include<string>
#include<vector>

// Function Prototypes

namespace var {
    unsigned long int getrandbufsize(void);

    void setbuffermax(unsigned long int setval);

    unsigned long int getbuffermax(void);

    void joinbuffer(void);

    void clearbuffer(void);

    bool changable(std::string iden);

    bool exists(std::string iden);

    void update(std::string iden, const std::string& val);
    
    std::string search(std::string iden, bool nothrow=false);

    std::string	mostrecent(void);

    unsigned long int count(void);

    int delvar(std::string variden);

    void delAll(void);

    std::vector<std::string> globals(void);

    std::vector<std::string> specials(void);

    void buffer(bool run);

}

void subVec(std::vector<std::string>& vec, unsigned long int start, unsigned long intend);

#endif