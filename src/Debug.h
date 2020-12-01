#include<vector>
#include<iostream>

namespace dbg {

	template<class T>
	void printvec(std::vector<T> print){
	std::cout << '[';
	for(unsigned long i=0; i < print.size(); i++){
		if(i >= 1){
			std::cout << ", ";
		}
   		std::cout << print.at(i);
	}
	std::cout << ']';
}
}