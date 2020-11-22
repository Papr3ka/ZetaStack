#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>

namespace var {
	void update(std::string iden, std::string val);
	
	std::string search(std::string iden);

	unsigned long int count(void);

	int delvar(std::string variden);

	std::vector<std::string> globals(void);

}

std::vector<std::string> subVec(std::vector<std::string> vec, unsigned long start, unsigned long end);

#endif