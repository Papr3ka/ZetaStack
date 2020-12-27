#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<iostream>

#include "Function.hpp"
#include "Zetacompiler.hpp"
#include "Token.hpp"

class func{
	private:
		long int argcnt = 0;
		std::string functionname;
		std::vector<token> functionargs;
		std::vector<token> functionbody;

	public:
		// Constructor
		func(std::vector<token> argvect, std::string name, std::vector<token> functokens){
			functionargs = argvect;
			for(token x: argvect){
				if(x.type != 7){
					argcnt++;
				}
			}

			functionname = name;
			functionbody = functokens;

		}
		// Deconstructor
		~func(void){}


		inline std::string fname(void){
			return functionname;
		}
		inline std::vector<token> ret(void){
			return functionbody;
		}

		inline long int argcount(void){
			return argcnt;
		}

		inline std::vector<token> getarg(void){
			return functionargs;
		}

};

token lookup(token var, std::vector<token> identifiers, std::vector<token> args){
	long int index = 0;
	for(token id: identifiers){
		if(id.data == var.data){
			return args.at(index);
		}
		index++;
	}
	return var;
}
/*
	argsname ex a, b, c : identifiers
	argsvar  ex 2, 4, 8 : Nums to replace identifiers 
	fbody    ex a+b+c   : the function
*/
std::vector<token> fillvars(std::vector<token> argsname, std::vector<token> argsvar, std::vector<token> fbody){
	if(argsname.size() == 0) return fbody; // Return if there are no arguments
	std::vector<token> output;
	token varfilldata;
	while(!fbody.empty()){ 
		switch(fbody.front().type){
			case 5:
				varfilldata = lookup(fbody.front(), argsname, argsvar);
				output.push_back(varfilldata);
				fbody.erase(fbody.begin());
				break;
			default:
				output.push_back(fbody.front());
				fbody.erase(fbody.begin());
				break;
		}
	}
	return output;
}



std::vector<func> nfunctions; // Callable normal functions

// Wrapper functions

// delete function
int udef(std::string name, long int argcounts){
	unsigned long int idx = 0;
	if(argcounts != -1){
		for(func f_id: nfunctions){
			if(f_id.fname() == name && f_id.argcount() == argcounts){
				nfunctions.erase(nfunctions.begin()+idx);
				return 0;
			}
			idx++;
		}
	}else{
		unsigned long int success = 0;
		for(func f_id: nfunctions){
			if(f_id.fname() == name){
				nfunctions.erase(nfunctions.begin()+idx);
				success++;
			}
			idx++;
		}
		if(success > 0) return 0;
	}
	return 1;
}

void def(std::vector<token> assignTo, std::vector<token> body){ // Input must go through lexical analyzer and tokenComp
	std::string name = assignTo.front().data;
	assignTo.erase(assignTo.begin());
	assignTo.erase(assignTo.begin());// Erase name and first bracket
	assignTo.pop_back(); // Erase end bracket
	udef(name, assignTo.size());
	func obj(assignTo,name,body); // Create function object
	nfunctions.push_back(obj);
	return;
}

bool fexists(std::string name, signed long int argcounts){
	for(func f_id: nfunctions){
		if(f_id.fname() == name && f_id.argcount() == argcounts){
			return true;
		}
	}
	return false;
}

// call function returns body list of tokens with vars filled
// format = funcname(, arg1, arg2, ...
std::vector<token> call(std::vector<token> fargs, std::string name){
	unsigned long int idx = 0;
	signed long int argsize = fargs.size();
	for(func f_id: nfunctions){
		if(f_id.fname() == name && f_id.argcount() == argsize ){
			return fillvars(nfunctions.at(idx).getarg(), fargs, nfunctions.at(idx).ret());
		}
		idx++;
	}
	std::vector<token> nullvec;
	return nullvec;
}

long int argcount(std::string name){
	unsigned long int idx = 0;
	for(func f_id: nfunctions){
		if(f_id.fname() == name){
			break;
		}
		idx++;
	}
	return nfunctions.at(idx).argcount();
}
