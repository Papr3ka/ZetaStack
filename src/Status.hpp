/* Header for Status.cpp
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

#ifndef STATUS_HPP
#define STATUS_HPP

#include "ZetaStack.hpp"

std::string strunc(float x, short prec);

std::string prec_str(double fstr);

std::string strunc(double x, unsigned long  int prec);

namespace bar {

    void setstate(bool mode);

    void init(long int max);

    void start(void);

    void stop(void);

    void join(void);

    void inform(std::string md);

    void changemode(int m);

    void setcount(float x);

    void setcount(unsigned long int x);

    void updatecycle(void);

    void updatepercent(void);

    void finish(void);

    void printblank(void);

    void barmanager(void);
}

#endif