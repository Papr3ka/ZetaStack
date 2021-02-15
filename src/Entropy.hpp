/* Header for Entropy.cpp
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

#ifndef ENTROPY_HPP
#define ENTROPY_HPP

#include<chrono>
#include<cstdint>
#include<thread>

uint32_t urandom(uint32_t seed);

void initseed(void);

uint64_t psuedo_random_next(void);

uint32_t getrandnum(uint32_t seed);

bool getrandbit(uint32_t seed);

uint32_t getrandl(uint32_t seed);

uint64_t getrandull(uint32_t seed);

#endif