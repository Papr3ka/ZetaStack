/* BuiltIn.cpp header
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

#include<string>
#include<unordered_map>
#include<vector>

#include "Execute.hpp"
#include "ZetaStack.hpp"

#ifndef BUILTIN_HPP
#define BUILTIN_HPP

extern unsigned long int CPU_COUNT;

/*--Unit-----Value--
*|  RAD     | 0    |
*|  GRAD    | 1    |
*|  DEG     | 2    |
*///----------------
extern unsigned char angle_unit;

// Variable ////////////////////////////////

extern std::unordered_map<std::string, std::string> specialIden;

// Function ////////////////////////////////

constexpr int TOTAL_BUILTIN_FUNCS = 32; // MUST be >= or else SIGSEGV on startup

constexpr int TOTAL_BUILTIN_CORE_FUNCS = 8; // MUST be >= or else SIGSEGV on startup


// Trigonomic

double b_sin(std::vector<token> x);

double b_cos(std::vector<token> x);

double b_tan(std::vector<token> x);

double b_asin(std::vector<token> x);
	
double b_acos(std::vector<token> x);
	
double b_atan(std::vector<token> x);

////////////////////////////////
// Hyperbolic

double b_sinh(std::vector<token> x);

double b_cosh(std::vector<token> x);

double b_tanh(std::vector<token> x);

double b_asinh(std::vector<token> x);

double b_acosh(std::vector<token> x);

double b_atanh(std::vector<token> x);

////////////////////////////////

double b_exp(std::vector<token> x);

////////////////////////////////

double b_ln(std::vector<token> x);

double b_log10(std::vector<token> x);

double b_log(std::vector<token> x);

////////////////////////////////

double b_sqrt(std::vector<token> x);

double b_cbrt(std::vector<token> x);

double b_root(std::vector<token> x);

////////////////////////////////

double b_sum(std::vector<token> x);

double b_prod(std::vector<token> x);

double b_avg(std::vector<token> x);

double b_min(std::vector<token> x);

double b_max(std::vector<token> x);

////////////////////////////////

double b_floor(std::vector<token> x);

double b_ceil(std::vector<token> x);

double b_round(std::vector<token> x);

double b_abs(std::vector<token> x);

////////////////////////////////

double b_zeta(std::vector<token> x);

double b_gamma(std::vector<token> x);

////////////////////////////////

double b_erf(std::vector<token> x);

double b_factorial(std::vector<token> x);

// CORE Function ////////////////////////////////

token bc_input(std::vector<token> x);

token bc_echo(std::vector<token> x);

token bc_abort(std::vector<token> x);

token bc_numcast(std::vector<token> x);

token bc_strcast(std::vector<token> x);

token bc_sleep(std::vector<token> x);

token bc_safe_sleep(std::vector<token> x);

#endif