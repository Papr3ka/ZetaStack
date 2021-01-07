#ifndef CACHE_HPP
#define CACHE_HPP

#include<string>
#include<vector>

extern bool do_cache;

// Function Prototypes

struct ansctable{
	std::string key;
	std::string ans;
	ansctable(std::string k, std::string a):
	key(k), ans(a)
	{}
};

namespace cch {

	void setmaxlen(unsigned long int x);

	void addentry(std::string key, std::string ans);

	std::string searchentry(std::string key);

	std::vector<ansctable> getentries(void);

	void clearcache(void);

}

#endif