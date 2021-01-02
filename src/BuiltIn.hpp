#include<string>
#include<vector>

#include "Token.hpp"

#ifndef BUILTIN_HPP
#define BUILTIN_HPP

extern unsigned long int CPU_COUNT;

// Variable ////////////////////////////////

extern std::unordered_map<std::string, std::string> specialIden;

// Function ////////////////////////////////

#define TOTAL_BUILTIN_FUNCS 26 // MUST be >= or else SIGSEGV on startup


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

double b_sum(std::vector<token> x);

double b_prod(std::vector<token> x);

double b_avg(std::vector<token> x);

////////////////////////////////

double b_floor(std::vector<token> x);

double b_ceil(std::vector<token> x);

double b_round(std::vector<token> x);

////////////////////////////////
	
double b_max(std::vector<token> x);

double b_min(std::vector<token> x);

////////////////////////////////

double b_zeta(std::vector<token> x);

double b_gamma(std::vector<token> x);

#endif