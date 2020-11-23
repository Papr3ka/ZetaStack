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


#include "Execute.hpp"
#include "Preprocessor.hpp"
#include "Zetacompiler.hpp"

// Global Settings
bool debug_mode = false; // Debugging
bool measure_time = false; // Measuring time
bool do_exec = true; // Execute

std::vector<std::string> tunit{
	" ns",
	" µs",
	" ms",
	" s"
}; // len 4

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
				if(it == identifier.end()){
					return "NULL";
				}else{
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
				std::cout << "Cache disabled\n";
			}else{
				enable = true;
				std::cout << "Cache enabled\n";
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

// commands using the "/" symbol
void command(std::string com){
	if(com[0] == '/'){
		return;
	}

	std::vector<std::string> cmdargv = split(com, " "); // split
	// index 0 is command. index > 0 is args for command or secondary command

	cmdargv.push_back(" "); // add space to make size > 1 in some cases to prevent SIGSEGV

	if(cmdargv[0] == "debug" || 
	   cmdargv[0] == "Debug" || 
	   cmdargv[0] == "dbg"){
		if(debug_mode){
			debug_mode = false;
			std::cout << "Debug off\n";
		}else{
			debug_mode = true;
			std::cout << "Debug on\n";
		}
	}else if(cmdargv[0] == "exit"){
		exit(0);
	}else if(cmdargv[0] == "Cache" || 
			 cmdargv[0] == "cache"){
		if(cmdargv[1] == "show"){
			cache.show();
		}else if(cmdargv[1] == "clear"){
			cache.reset();
			std::cout << "Cache cleared\n";
		}else if(cmdargv[1] == "toggle"){
			cache.toggle();
		}else{
			std::cout << "Options for command \"" << 
						cmdargv[0] << "\"\n   " << 
						cmdargv[0] <<" show\n   " << 
						cmdargv[0] <<" clear\n   " <<
						cmdargv[0] <<" toggle\n";
		}
	}else if(cmdargv[0] == "del"){
		int delsuccess = 0;
		std::vector<std::string> vgls = var::globals();
		for(auto x: vgls){
			if(x == cmdargv[1]){
				delsuccess += var::delvar(cmdargv[1]);
			}
		}
		if(delsuccess >= 1){
			std::cout << "Undefined variable: \"" << cmdargv[1] << "\"\n";
		}
	}else if(cmdargv[0] == "time"){
		if(measure_time){
			measure_time = false;
			std::cout << "Time off\n";
		}else{
			measure_time = true;
			std::cout << "Time on\n";

		}
	}else if(cmdargv[0] == "exec"){
		if(do_exec){
			do_exec = false;
			std::cout << "Execute off\n";
		}else{
			do_exec = true;
			std::cout << "Execute on\n";
		}
	}else{

		// Command not found
		std::cout << "Unkown Command: " << com << "\n";
	}

}

std::string calcExecuter(std::string input){
	int tscale;
	double totaltime;
	int bracMis = 0;

	std::chrono::_V2::steady_clock::time_point lex_tstart;
	std::chrono::_V2::steady_clock::time_point lex_tend;
	std::chrono::_V2::steady_clock::time_point tcomp_tstart;
	std::chrono::_V2::steady_clock::time_point tcomp_tend;
	std::chrono::_V2::steady_clock::time_point shyd_tstart;
	std::chrono::_V2::steady_clock::time_point shyd_tend;
	std::chrono::_V2::steady_clock::time_point exec_tstart;
	std::chrono::_V2::steady_clock::time_point exec_tend;

	std::vector<std::string> output;
	std::string finalOutput;
	lex_tstart = std::chrono::steady_clock::now();	
	output = comp::lex(input);	
	lex_tend = std::chrono::steady_clock::now();
	if(debug_mode){
		std::cout << "Lexer:        ";
		printvec(output);
		std::cout << "\n";
	}	
	tcomp_tstart = std::chrono::steady_clock::now();	
	output = comp::tokenComp(output);	
	tcomp_tend = std::chrono::steady_clock::now();	
	if(debug_mode){
		std::cout << "TokenComp:    ";
		printvec(output);
		std::cout << "\n";
	}
	bracMis = comp::checkBrac(output);
	if(bracMis == 0){	
		shyd_tstart = std::chrono::steady_clock::now();
		output = comp::shuntingYard(output);
		shyd_tend = std::chrono::steady_clock::now();					
		if(debug_mode){
			std::cout << "ShuntingYard: ";
			printvec(output);
			std::cout << "\n\n";
		}
		auto it = std::find(output.begin(), output.end(), "NULL");
		if(it != output.end()){
			return "Undefined Variable";
		}
		if(do_exec){
			cache.update(input, finalOutput);
		}
		
		if(do_exec){
			exec_tstart = std::chrono::steady_clock::now();
			finalOutput = xmath::calculate(output);
			exec_tend = std::chrono::steady_clock::now();
		}else{
			exec_tstart = std::chrono::steady_clock::now();
			exec_tend = exec_tstart;
		}
		double lextime = std::chrono::duration<double, std::nano>(lex_tend - lex_tstart).count();
		double tcomptime = std::chrono::duration<double, std::nano>(tcomp_tend - tcomp_tstart).count();
		double shydtime = std::chrono::duration<double, std::nano>(shyd_tend - shyd_tstart).count();
		double exectime = std::chrono::duration<double, std::nano>(exec_tend - exec_tstart).count();
		totaltime = lextime+tcomptime+shydtime+exectime;
		if(totaltime < 1000){
			tscale = 0;
		}else if(totaltime >= 1000 && totaltime < 1000000){
			tscale = 1;
			lextime /= 1000;
			tcomptime /= 1000;
			shydtime /= 1000;
			exectime /= 1000;
			totaltime /= 1000;
		}else if(totaltime >= 1000000 && totaltime < 1000000000){
			tscale = 2;
			lextime /= 1000000;
			tcomptime /= 1000000;
			shydtime /= 1000000;
			exectime /= 1000000;
			totaltime /= 1000000;
		}else{
			tscale = 3;
			lextime /= 1000000000;
			tcomptime /= 1000000000;
			shydtime /= 1000000000;
			exectime /= 1000000000;
			totaltime /= 1000000000;
		}
		if(measure_time){
		std::cout << "Time variable:\n   Lexer:        " << lextime << 
					 tunit[tscale] << " \n   TokenComp:    " << tcomptime  <<
					 tunit[tscale] << " \n   ShuntingYard: " << shydtime << 
					 tunit[tscale] << " \n   Execution:    " << exectime << 
					 tunit[tscale] << " \n   Total:        " << totaltime <<
					 tunit[tscale] << " \n\n";
		}
	}else{
		std::cout << "Bracket Mismatch\n";
		
		finalOutput = "ERR";
	}
	return finalOutput;
}

std::string assingExecuter(std::string input, int etype){
	int tscale;
	double totaltime;
	int bracMis = 0;

	std::chrono::_V2::steady_clock::time_point lex_tstart;
	std::chrono::_V2::steady_clock::time_point lex_tend;
	std::chrono::_V2::steady_clock::time_point tcomp_tstart;
	std::chrono::_V2::steady_clock::time_point tcomp_tend;
	std::chrono::_V2::steady_clock::time_point shyd_tstart;
	std::chrono::_V2::steady_clock::time_point shyd_tend;
	std::chrono::_V2::steady_clock::time_point exec_tstart;
	std::chrono::_V2::steady_clock::time_point exec_tend;

	std::vector<std::string> partasn;
	std::vector<std::string> output;
	std::string finalOutput;

	partasn = comp::spliteq(input);
	input = partasn.back();

	lex_tstart = std::chrono::steady_clock::now();	
	output = comp::lex(input);	
	lex_tend = std::chrono::steady_clock::now();
	if(debug_mode){
		std::cout << "Lexer:        ";
		printvec(output);
		std::cout << "\n";
	}	
	tcomp_tstart = std::chrono::steady_clock::now();	
	output = comp::tokenComp(output);	
	tcomp_tend = std::chrono::steady_clock::now();	
	if(debug_mode){
		std::cout << "TokenComp:    ";
		printvec(output);
		std::cout << "\n";
	}

	bracMis = comp::checkBrac(output);
	if(bracMis == 0){	
		shyd_tstart = std::chrono::steady_clock::now();
		output = comp::shuntingYard(output);
		shyd_tend = std::chrono::steady_clock::now();
		if(etype != 0){	
			output.push_back(comp::getop(etype - 1));
			output.insert(output.begin(), var::search(partasn.front()));
		}
		if(debug_mode){
			std::cout << "ShuntingYard: ";
			printvec(output);
			std::cout << "\n\n";
		}
		auto it = std::find(output.begin(), output.end(), "NULL");
		if(it != output.end()){
			return "Undefined Variable";
		}
		
		if(do_exec){
			exec_tstart = std::chrono::steady_clock::now();
			finalOutput = xmath::calculate(output);
			exec_tend = std::chrono::steady_clock::now();
		}else{
			exec_tstart = std::chrono::steady_clock::now();
			exec_tend = exec_tstart;
		}
		double lextime = std::chrono::duration<double, std::nano>(lex_tend - lex_tstart).count();
		double tcomptime = std::chrono::duration<double, std::nano>(tcomp_tend - tcomp_tstart).count();
		double shydtime = std::chrono::duration<double, std::nano>(shyd_tend - shyd_tstart).count();
		double exectime = std::chrono::duration<double, std::nano>(exec_tend - exec_tstart).count();
		totaltime = lextime+tcomptime+shydtime+exectime;
		if(totaltime < 1000){
			tscale = 0;
		}else if(totaltime >= 1000 && totaltime < 1000000){
			tscale = 1;
			lextime /= 1000;
			tcomptime /= 1000;
			shydtime /= 1000;
			exectime /= 1000;
			totaltime /= 1000;
		}else if(totaltime >= 1000000 && totaltime < 1000000000){
			tscale = 2;
			lextime /= 1000000;
			tcomptime /= 1000000;
			shydtime /= 1000000;
			exectime /= 1000000;
			totaltime /= 1000000;
		}else{
			tscale = 3;
			lextime /= 1000000000;
			tcomptime /= 1000000000;
			shydtime /= 1000000000;
			exectime /= 1000000000;
			totaltime /= 1000000000;
		}
		if(measure_time){
		std::cout << "Time variable:\n   Lexer:        " << lextime << 
					 tunit[tscale] << " \n   TokenComp:    " << tcomptime  <<
					 tunit[tscale] << " \n   ShuntingYard: " << shydtime << 
					 tunit[tscale] << " \n   Execution:    " << exectime << 
					 tunit[tscale] << " \n   Total:        " << totaltime <<
					 tunit[tscale] << " \n\n";
		}
	}else{
		std::cout << "Bracket Mismatch\n";
		
		finalOutput = "ERR";
	}
	var::update(partasn.front(),finalOutput);
	return finalOutput;
}

int main(int argc, char* argv[]){

	int runtype;

	std::cout << "ZetaStack\n";
	std::string input;
	std::string output;
	std::string finalOutput;
	std::string newline = ">>> ";

	

	while(true){
		std::cout << newline;
		std::getline(std::cin, input);

		input.push_back(' '); // prevent SIGSEGV by making sure len > 2

		if(input[0] != '/'){

			// remove all whitespace and strip comments
		 	// Preprocessor.hpp
			input = comp::removeWhiteSpace(input);
			input = comp::stripcomment(input);

			if(input == ""){
				continue;
			}
			/*
				-1 Error
				0 Calculate
				1 Assign
				2 Bool Compare
				3 Calculate-Assign
			*/
			runtype = comp::execType(input);
			if(cache.search(input) == "NULL"){

				switch(runtype){
					case 0:
						finalOutput = calcExecuter(input);						
						break;
					case 1:
						/*
							-1 Error
							0 ASN     =
							1 ADDASN  +=
							2 SUBASN  -=
							3 MULASN  *=
							4 DIVASN  /=
							5 MODASN  %=
							6 XORASN  ^=
							7 POWASN  **=
							8 SHLASN  <<=
							9 SHRASN  >>=
						*/
						finalOutput = assingExecuter(input, comp::assigntype(input));
						break;
				}
				

			}else{
				finalOutput = cache.search(input);
				if(debug_mode){
					std::cout << "Cache Hit\n\n";
				}
			}
			if(do_exec){		
				std::cout << finalOutput << "\n";
			}
		}else{
			// commands
			input.erase(input.begin());
			command(input);
		}
	}
	return 0;
}