#ifndef TOKEN_HPP
#define TOKEN_HPP

#include<string>

// Main data type to be processed

/*
typedef struct{
	std::string data;
	int type;
}token;
*/

/*
	0 - NUM
	1 - OPERATOR
	2 - LEFT BRACKET
	3 - RIGHT BRACKET
	4 - FUNCTION
	5 - VARIABLE
	6 - R FUNC
	7 - Separator / Func End
*/	

class token{
	private:
	public:

		std::string data;
		int type;

		// overloaded constructors for optional parameters
		token(){}
		token(std::string d, int t){ 
			data = d;
			type = t;
		}
		~token(){}

		// comparisons
		friend bool operator != (token a, token b){
			if(a.data != b.data) return false;
			if(a.type != b.type) return false;
			return true;
		}

		friend bool operator == (token a, token b){
			if(a.data == b.data) return false;
			if(a.type == b.type) return false;
			return true;
		}

		// Required for <string> to compare token with const char []
		friend bool operator == (token x, const char mval[]){
			if(x.data.c_str() == mval) return true;
			return false;
		}
};

#endif