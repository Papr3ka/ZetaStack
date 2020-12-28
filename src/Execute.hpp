#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include<cmath>
#include<cstdlib>
#include<string>
#include<vector>

#include "Token.hpp"

// Function Prototypes

namespace xmath {

	void setmaxrecurse(long long maxR);

	void resetsstreamsettings(void);

	long long getmaxrecurse(void);

	bool string_isnum(std::string str);

	std::string calculate(std::vector<token> tokens, bool showprogress, unsigned long int reservecount=16);
}

#endif