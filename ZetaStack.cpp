#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<cctype>
#include<chrono>
#include<ratio>

#include<gmpxx.h>

#include<cstdlib>
#include<cstdio>
#include<cassert>
#include<cmath>

#include "Zetacompiler.hpp"
#include "Execute.hpp"

#define True 1
#define False 0

// Global Settings
bool debug_mode = false; // Debugging
bool measure_time = false; // Measuring time

class vectorDictionary{
	private:
		bool enable = true;
		std::vector <std::string> identifier;
		std::vector <std::string> value;
	public:
		void update(std::string iden, std::string val){
			if(enable){
				identifier.push_back(iden);
				value.push_back(val);
			}
		}
		std::string search(std::string iden){
			if(enable){
				auto it = std::find(identifier.begin(), identifier.end(), iden);
				if (it == identifier.end())
				{
				  return "NULL";
				} else
				{
				  int index = std::distance(identifier.begin(), it);
				  return value[index];
				}
			}else{
				return "NULL";
			}
		}
		void show(void){
			std::cout << "Size: " << identifier.size() << "\n----------\n";
			for(unsigned long int idx=0; idx < identifier.size(); idx++){
				std::cout << "\"" << identifier[idx] << "\":  \"" << value[idx] << "\"\n";
			}
		}

		void reset(void){
			identifier.clear();
			value.clear();
		}

		void toggle(void){
			if(enable){
				enable = false;
			}else{
				enable = true;
			}
		}

};
vectorDictionary cache;

void printvec(std::vector<std::string> print){
	std::cout << '[';
	for(unsigned long i=0; i < print.size(); i++){
		if(i >= 1){
			std::cout << ", ";
		}
   		std::cout << print.at(i);
	}
	std::cout << ']';
}

std::vector<std::string> split (std::string str, std::string split) {
    size_t start = 0, end, splitLen = split.size();
    std::string token;
    std::vector<std::string> output;
    while ((end = str.find(split, start)) != std::string::npos) {
        token = str.substr(start, end - start);
        start = end + splitLen;
        output.push_back(token);
    }
    output.push_back(str.substr(start));
    return output;
}

// commands using the "/" symbol
void command(std::string com){
	std::vector<std::string> cmdargv = split(com, " "); // split
	// index 0 is command. index > 0 is args for command or secondary command

	cmdargv.push_back(" "); // add space to make size > 1 in some cases to prevent SIGSEGV

	if(cmdargv[0] == "debug" || 
	   cmdargv[0] == "Debug" || 
	   cmdargv[0] == "dbg"){
		if(debug_mode){
			debug_mode = false;
		}else{
			debug_mode = true;
		}
	}else if(cmdargv[0] == "exit"){
		exit(0);
	}else if(cmdargv[0] == "Cache" || 
			 cmdargv[0] == "cache"){
		if(cmdargv[1] == "show"){
			cache.show();
		}else if(cmdargv[1] == "reset"){
			cache.reset();
		}else if(cmdargv[1] == "toggle"){
			cache.toggle();
		}else{
			std::cout << "Options for command \"" << 
						cmdargv[0] << "\"\n   " << 
						cmdargv[0] <<" show\n   " << 
						cmdargv[0] <<" reset\n   " <<
						cmdargv[0] <<" toggle\n";
		}
	}else if(cmdargv[0] == "time"){
		if(measure_time){
			measure_time = false;
		}else{
			measure_time = true;
		}
	}else{

		// Command not found
		std::cout << "Unkown Command: " << com;
	}

}


int main(int argc, char* argv[]){

	std::cout << "PowerCalc\n";

	std::string input;

	std::vector<std::string> output;
	std::string finalOutput;

	std::string newline = "\n>>> ";

	int bracMis = 0;
	while(True){
		std::cout << newline;
		std::getline(std::cin, input);
		if(input == ""){
			continue;
		}
		if(input[0] != '/'){
			input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end()); // remove all whitespace
			if(cache.search(input) == "NULL"){
				auto lex_tstart = std::chrono::steady_clock::now();	
				output = comp::lex(input);	
				auto lex_tend = std::chrono::steady_clock::now();
				if(debug_mode){
					std::cout << "Lexer: ";
					printvec(output);
					std::cout << "\n";
				}	
				auto tcomp_tstart = std::chrono::steady_clock::now();	
				output = comp::tokenComp(output);	
				auto tcomp_tend = std::chrono::steady_clock::now();	
				if(debug_mode){
					std::cout << "TokenComp: ";
					printvec(output);
					std::cout << "\n";
				}
				bracMis = comp::checkBrac(output);
				double lextime = std::chrono::duration_cast<std::chrono::microseconds>(lex_tend - lex_tstart).count();
				double tcomptime = std::chrono::duration_cast<std::chrono::microseconds>(tcomp_tend - tcomp_tstart).count();
				if(bracMis == 0){	
					auto shyd_tstart = std::chrono::steady_clock::now();
					output = comp::shuntingYard(output);
					auto shyd_tend = std::chrono::steady_clock::now();					
					if(debug_mode){
						std::cout << "ShuntingYard: ";
						printvec(output);
						std::cout << "\n\n";
					}
					auto exec_tstart = std::chrono::steady_clock::now();
					finalOutput = xmath::calculate(output);
					auto exec_tend = std::chrono::steady_clock::now();
					double shydtime = std::chrono::duration_cast<std::chrono::microseconds>(shyd_tend - shyd_tstart).count();
					double exectime = std::chrono::duration_cast<std::chrono::microseconds>(exec_tend - exec_tstart).count();
					if(measure_time){
					std::cout << "Time variable:\n   Lexer: " << lextime << 
								 " µs\n   TokenComp: " << tcomptime  <<
								 " µs\n   ShuntingYard: " << shydtime << 
								 " µs\n   Execution: " << exectime << 
								 " µs\n   Total: " << lextime+tcomptime+shydtime+exectime <<
								 " µs\n\n";
					}
				}else{
					std::cout << "Bracket Mismatch\n";
					if(debug_mode){
						std::cout << bracMis << "\n";
					}
				}
				cache.update(input, finalOutput);
			}else{
				finalOutput = cache.search(input);
				if(debug_mode){
					std::cout << "Cache Hit\n\n";
				}
			}
			if(bracMis == 0){		
				std::cout << finalOutput;
			}
		}else{
			// commands
			input.erase(input.begin());
			command(input);
		}
	}
	return 0;
}