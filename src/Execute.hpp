#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>

#include "Token.hpp"

// Function Prototypes

namespace xmath {

	void setmaxrecurse(long long maxR);

	void resetsstreamsettings(void);

	long long getmaxrecurse(void);

	bool string_isnum(std::string str);

	long long factorial(long long x);

	std::string calculate(std::vector<token> tokens, bool showprogress);

	//std::vector<std::string> callall(std::vector<std::string> tokens);
}

#endif