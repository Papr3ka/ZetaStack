#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<gmpxx.h>

// Function Prototypes

namespace xmath {

	bool string_isnum(std::string str);

	double add(double x, double y);

	double sub(double x, double y);

	double mul(double x, double y);

	double divide(double x, double y);

	double mod(double x, double y);

	double power(double x, double y);

	double exclusiveOr(double x, double y);

	double floordiv(double x, double y);

	long long shl(long long x, long long y);

	long long shr(long long x, long long y);

	long long factorial(long long x);

	std::string calculate(std::vector<std::string> tokens);

	//std::vector<std::string> callall(std::vector<std::string> tokens);
}

#endif