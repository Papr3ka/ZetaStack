#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>


// Function Prototypes

std::vector<std::string> split(std::string str, std::string split);

namespace comp {

	std::vector<std::string> lex(std::string lexInput);

	std::vector<std::string> tokenComp(std::vector<std::string> tokensInput);

	int assigntype(std::string str);
	
	std::string removeWhiteSpace(std::string str);

	std::string stripcomment(std::string str);

	int execType(std::string str);

	std::vector<std::string> spliteq(std::string str);
}

#endif
