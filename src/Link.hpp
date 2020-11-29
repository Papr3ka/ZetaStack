#ifndef LINK_HPP
#define LINK_HPP

#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>

namespace ld {

	std::vector<std::string> link(std::vector<std::string> tokens);

	std::vector<std::string> recurselink(std::vector<std::string> tokens, long long int recursecount = 0);

}

#endif