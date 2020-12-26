#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<string>
#include<vector>

// Function Prototypes

namespace var {
	unsigned long int getrandbufsize(void);

	void setbuffermax(unsigned long int setval);

	unsigned long int getbuffermax(void);

	void joinbuffer(void);

	void clearbuffer(void);

	bool changable(std::string iden);

	bool exists(std::string iden);

	void update(std::string iden, std::string val);
	
	std::string search(std::string iden, bool nothrow=false);

	std::string	mostrecent(void);

	unsigned long int count(void);

	int delvar(std::string variden);

	std::vector<std::string> globals(void);

	std::vector<std::string> specials(void);

	void buffer(bool run);

}

std::vector<std::string> subVec(std::vector<std::string> vec, unsigned long start, unsigned long end);

#endif