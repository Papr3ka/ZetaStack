#ifndef CACHE_HPP
#define CACHE_HPP

#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

// Function Prototypes

namespace cch {
	std::vector<std::string> getiden(void);

	std::vector<std::string> getval(void);

	void setstate(bool state);

	bool getstate(void);	

	void setmaxlen(unsigned long int ml);

	unsigned long int getmaxlen(void);

	void reset(void);

	void update(std::string iden, std::string val, std::vector<std::string> depends);

	void refreshDepends(std::string change);

	std::string search(std::string iden);

}

#endif