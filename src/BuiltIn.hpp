#include<string>
#include<unordered_map>
#include<vector>

#include "Token.hpp"

#ifndef BUILTIN_HPP
#define BUILTIN_HPP

extern unsigned long int CPU_COUNT;

extern bool do_bar;

/*--Unit-----Value--
*|  RAD     | 0    |
*|  GRAD    | 1    |
*|  DEG     | 2    |
*///----------------
extern unsigned char angle_unit;

// Variable ////////////////////////////////

extern std::unordered_map<std::string, std::string> specialIden;

// Function ////////////////////////////////

#define TOTAL_BUILTIN_FUNCS 31 // MUST be >= or else SIGSEGV on startup

#define TOTAL_BUILTIN_CORE_FUNCS 7 // MUST be >= or else SIGSEGV on startup


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

#endif