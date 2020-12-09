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
		unsigned long int argcnt;
		std::string functionname;
		std::vector<token> tempfunc;
		std::vector<token> functionargs;
		std::vector<token> functionargsNosep;
		std::vector<token> functionbody;
		/*	Reference table guide
		|	>= 0 index of argument in compiled function
		|	-1 skip/continue to next argument
		V	-2 end of arguments
		*/
		std::vector<long> referenceTable;

	public:
		// Constructor
		func(std::vector<token> argvect, std::string name, std::vector<token> functokens){
			functionargs = argvect;
			for(token x: argvect){
				if(x.type != -1){
					functionargsNosep.push_back(x);
				}
			}

			functionname = name;
			functionbody = functokens;
			argcnt = argvect.size() - 1;
			if(argcnt == 0){
				referenceTable.push_back(-2);
			}
			//dbg::printvec(functionargsNosep);
			//std::cout << " args\n";

		}
		// Deconstructor
		~func(void){}

		void reference(void){
			referenceTable.clear();
			// Map argument to token in compiled function
			unsigned long int idx;
			for(token argiden: functionargsNosep){
				for(idx = 0 ;idx < functionbody.size(); idx++){
					if(functionbody[idx].data == argiden.data){
						referenceTable.push_back(idx);
					}
				}
				referenceTable.push_back(-1);
			}
			referenceTable.pop_back();
			referenceTable.push_back(-2);
			//std::cout << "\nref"; dbg::printvecl(referenceTable);		
			return;	
		}

		void compile(void){
			functionbody = comp::shuntingYard(functionbody);
			reference();
			return;
		}

		// returns vector with variables used in function filled
		std::vector<token> fillvars(std::vector< std::vector<token> > varargs){
			std::vector<token> tempfunc(functionbody.begin(),functionbody.end());
			long argcounter = 0;
			reference();
			if(argcnt >= 1){ // check if function arg is not void
				for(long r_index: referenceTable){
					// Fill
					if(r_index == -1){
						argcounter++;
					}else if(r_index == -2){
						return tempfunc;
					}else{
						tempfunc.erase(tempfunc.begin() + r_index);// r_index
						tempfunc.insert(tempfunc.begin() + r_index, varargs[argcounter].begin(), varargs[argcounter].end());
					}
				}
				
			}
			return tempfunc; // error
		}

		std::string fname(void){
			return functionname;
		}
		std::vector<token> ret(void){
			return functionbody;
		}

		unsigned long int argcount(void){
			return argcnt;
		}

		void cleartemp(void){
			std::vector<token>().swap(tempfunc);
			return;
		}
};

std::vector<func> nfunctions; // Callable normal functions

// Wrapper functions

// delete function
void udef(std::string name){
	unsigned long int idx = 0;
	for(func f_id: nfunctions){
		if(f_id.fname() == name){
			nfunctions.erase(nfunctions.begin()+idx);
		}
		idx++;
	}
}

void def(std::vector<token> assignTo, std::vector<token> body){ // Input must go through lexical analyzer and tokenComp
	std::string name = assignTo.front().data;
	udef(name);
	assignTo.erase(assignTo.begin()); assignTo.erase(assignTo.begin());// Erase name and first bracket
	assignTo.pop_back(); // Erase end bracket
	func obj(assignTo,name,body); // Create function object
	//obj.compile();
	nfunctions.push_back(obj);	

}


// call function returns body list of tokens with vars filled
// format = funcname(, arg1, arg2, ...
std::vector<token> call(std::vector< std::vector<token> > fargs, std::string name){
	unsigned long int idx = 0;
	for(func f_id: nfunctions){
		if(f_id.fname() == name){
			break;
		}
		idx++;
	}

	return nfunctions.at(idx).fillvars(fargs);
}


unsigned long int argcount(std::string name){
	unsigned long int idx = 0;
	for(func f_id: nfunctions){
		if(f_id.fname() == name){
			break;
		}
		idx++;
	}
	return nfunctions.at(idx).argcount();
}

// clear tempdata in function
void clearfuncdata(std::string name){
	unsigned long int idx = 0;
	for(func f_id: nfunctions){
		if(f_id.fname() == name){
			break;
		}
		idx++;
	}	
	nfunctions.at(idx).cleartemp();
	return;
}
