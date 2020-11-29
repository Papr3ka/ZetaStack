#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<iostream>

#include "Function.hpp"
#include "Zetacompiler.hpp"

class func{
	private:
		unsigned long int argcnt;
		std::string functionname;
		std::vector<std::string> tempfunc;
		std::vector<std::string> functionargs;
		std::vector<std::string> functionargsNosep;
		std::vector<std::string> functionbody;
		/*	Reference table guide
		|	>= 0 index of argument in compiled function
		|	-1 skip/continue to next argument
		V	-2 end of arguments
		*/
		std::vector<long> referenceTable;

	public:
		// Constructor
		func(std::vector<std::string> argvect, std::string name, std::vector<std::string> functokens){
			functionargs = argvect;
			for(std::string x: argvect){
				if(x != "SEP"){
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
			for(std::string argiden: functionargsNosep){
				for(idx = 0 ;idx < functionbody.size(); idx++){
					if(functionbody[idx] == argiden){
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
		std::vector<std::string> fillvars(std::vector<std::string> varargs){
			std::vector<std::string> tempfunc(functionbody.begin(),functionbody.end());
			std::vector<std::string> argumentvector;
			//dbg::printvec(tempfunc);
			//std::cout << " tempfunc\n";
			reference();
			//dbg::printvecl(referenceTable);
			//std::cout << " ref\n";
			if(argcnt >= 1){
				for(long r_index: referenceTable){
					//dbg::printvec(argumentvector);
					//std::cout << " argumentvector\n";
					if(r_index == -1){
						while(varargs.front() != "SEP" && varargs.size() >= 1){
							varargs.erase(varargs.begin());
						}
					}else if(r_index >= 0){
						for(unsigned long int ch=0; ch < varargs.size(); ch++){
							if(varargs[ch] != "SEP" && !(ch + 1 >= varargs.size())){
								argumentvector.push_back(varargs[ch]);
							}else{
								argumentvector = comp::shuntingYard(argumentvector, true);
								tempfunc.erase(tempfunc.begin()+ r_index);
								tempfunc.insert(tempfunc.begin() + r_index, argumentvector.begin(), argumentvector.end());
								argumentvector.clear();
								break;
							}
						}
						
					}else{
						return tempfunc;
					}
				}
				
			}
			return tempfunc; // error
		}

		std::string fname(void){
			return functionname;
		}
		std::vector<std::string> ret(void){
			return functionbody;
		}

		unsigned long int argcount(void){
			return argcnt;
		}

		void cleartemp(void){
			tempfunc.clear();
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

void def(std::vector<std::string> assignTo, std::vector<std::string> body){ // Input must go through lexical analyzer and tokenComp
	std::string name = assignTo.front();
	udef(name);
	assignTo.erase(assignTo.begin()); assignTo.erase(assignTo.begin());// Erase name and first bracket
	assignTo.pop_back(); // Erase end bracket
	func obj(assignTo,name,body); // Create function object
	obj.compile();
	nfunctions.push_back(obj);	

}


// call function returns compiled list of tokens that can be inserted
// format = arg1, arg2 ... , funcname(
std::vector<std::string> call(std::vector<std::string> fargs){
	std::string name = fargs.back();
	//fargs.erase(fargs.begin());
	fargs.pop_back();
	fargs.push_back("SEP");
	unsigned long int idx = 0;
	for(func f_id: nfunctions){
		if(f_id.fname() == name){
			break;
		}
		idx++;
	}
	//dbg::printvec(nfunctions.at(idx).fillvars(fargs));
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