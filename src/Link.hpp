#ifndef LINK_HPP
#define LINK_HPP

#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>

#include "Token.hpp"

namespace ld {

	std::vector<token> link(std::vector<token> tokens);

	std::vector<token> recurselink(std::vector<token> tokens, long long int recursecount = 0);

}

#endif