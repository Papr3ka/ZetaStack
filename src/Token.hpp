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
	8
	9 - String
	10 - assign operator
*/	

class token{
	private:
	public:

		std::string data;
		long int type;
		long int reserved = -1;

		// overloaded constructors for optional parameters
		inline token(){}
		inline token(std::string d, int t){ 
			data = d;
			type = t;
		}
		inline token(std::string d, int t, int res){
			data = d;
			type = t;
			reserved = res;
		}
		inline ~token(){}

		// comparisons
		inline friend bool operator != (token a, token b){
			if(a.data != b.data) return false;
			if(a.type != b.type) return false;
			return true;
		}

		inline friend bool operator == (token a, token b){
			if(a.data == b.data) return false;
			if(a.type == b.type) return false;
			return true;
		}

		// Required for <string> to compare token with const char []
		inline friend bool operator == (token x, const char mval[]){
			if(x.data.c_str() == mval) return true;
			return false;
		}

		inline std::string pass_unary(void){
			if(reserved == -1) return data;
			return data.substr(reserved + 1, data.size()-1);
		}
};

#endif