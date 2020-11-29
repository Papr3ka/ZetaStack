/*
  Copyright (C) 2020, Benjamin Yao
*/
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<chrono>
#include<ratio>

#include<gmpxx.h>

#include<cstdlib>
#include<cstdio>
#include<cassert>
#include<cmath>
#include<cctype>

#include "Preprocessor.hpp"
#include "Zetacompiler.hpp"
#include "Function.hpp"
#include "Link.hpp"
#include "Execute.hpp"

#include "Cache.hpp"

// Global Settings
bool debug_mode = false; // Debugging 
bool measure_time = false; // Measuring time
bool do_exec = true; // Execute

// Defualt settings
/*--Variable------------Value--
*|  debug_mode        | false |
*|  measure_time      | false |
*|  do_exec = true    | true  |
*///---------------------------

std::vector<std::string> tunit{
	" ns",
	" µs",
	" ms",
	" s"
}; // len 4

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
			std::vector<std::string> iden = cch::getiden();
			std::vector<std::string> val = cch::getval();
			std::cout << "Size: " << val.size() << "\n--------\n";
			for(unsigned long int idx=0; idx < iden.size(); idx++){
				std::cout << "\"" << iden[idx] << "\": \"" << val[idx] << "\"\n";
			}
		}else if(cmdargv[1] == "clear"){
			cch::reset();
			std::cout << "Cache cleared\n";
		}else if(cmdargv[1] == "toggle"){
			if(cch::getstate()){
				cch::setstate(false);
				std::cout << "Cache disabled\n";
			}else{
				cch::setstate(true);
				std::cout << "Cache enabled\n";
			}
		}else{
			std::cout << "Options for command \"" << 
						cmdargv[0] << "\"\n   " << 
						cmdargv[0] << " show\n   " << 
						cmdargv[0] << " clear\n   " <<
						cmdargv[0] << " toggle\n";
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

/*-Interpreting process order-----------------------------------------------Resposible Files-------------------------
*|  1. Lexical Analysis "2*(14+12)" -> [2,*,(,14,+,12,)] 		|	Preprocessor.hpp / Preprocessor.cpp |
*|  2. TokenComp [2,MUL,L_BRAC,14,ADD,12,R_BRAC] <J			|	Preprocessor.hpp / Preprocessor.cpp |
*|  3. ShuntingYard  [2, 14, 12, ADD, MUL] <J				|	Zetacompiler.hpp / Zetacompiler.cpp |
*|  4. Recursive Function link, replaces functions with their bodies	|	Link.hpp / Link.cpp		    |
*|  5. Fill all leftover variables					|	Zetacompiler.hpp / Zetacompiler.cpp |
*|  6. Execute 								|	Execute.hpp / Execute.cpp	    |
*///-----------------------------------------------------------------------------------------------------------------

// Directly executing statements
std::string calcExecuter(std::string input){
	bool writecache = true;
	int tscale;
	double totaltime;

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

	if(comp::hasvar(output)){
		writecache = false;
	}
	shyd_tstart = std::chrono::steady_clock::now();
	output = comp::shuntingYard(output, true); // Zetacompiler.hpp
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

	output = ld::recurselink(output); // Link.hpp

	output = comp::fillallvars(output); // Zetacompiler.hpp

	if(do_exec){
		exec_tstart = std::chrono::steady_clock::now();
		finalOutput = xmath::calculate(output);
		exec_tend = std::chrono::steady_clock::now();
		if(writecache){
			cch::update(input, finalOutput);
		}
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
	return finalOutput;
}

// execute then assign statement
std::string asnExecuter(std::string input, int etype){
	int tscale;
	double totaltime;

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

	shyd_tstart = std::chrono::steady_clock::now();
	output = comp::shuntingYard(output); // Zetacompiler.hpp
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

	output = ld::recurselink(output); // Link.hpp

	output = comp::fillallvars(output); // Zetacompiler.hpp

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
	if(do_exec){
		var::update(partasn.front(),finalOutput);
	}
	return finalOutput;
}

// Define a function
void deffunction(std::string input){
	std::vector<std::string> partasn = comp::spliteq(input);
	partasn.back() = comp::removeWhiteSpace(partasn.back());
	std::vector<std::string> funchead = comp::lex(partasn.front());
	std::vector<std::string> funcbody = comp::lex(partasn.back());
	if(debug_mode){
		std::cout << "Lexer:        [";
		for(std::string x: funchead){
			std::cout << x << ", ";
		}
		std::cout << "=, ";
		for(unsigned long int x = 0; x < funcbody.size(); x++){

			std::cout << funcbody[x];
			if(x + 1 < funcbody.size()){
				std::cout << ", ";
			}
		}
		std::cout << "]\n";
	}
	funchead = comp::tokenComp(funchead);
	funcbody = comp::tokenComp(funcbody);
	if(debug_mode){
		std::cout << "TokenComp:    [";
		for(std::string x: funchead){
			std::cout << x << ", ";
		}
		std::cout << "=, ";
		for(unsigned long int x = 0; x < funcbody.size(); x++){

			std::cout << funcbody[x];
			if(x + 1 < funcbody.size()){
				std::cout << ", ";
			}
		}
		std::cout << "]\n";
	}
	def(funchead, funcbody);
	return;
}

// Main entry point
int main(int argc, char* argv[]){

	int runtype;

	int lbcnt;
	int rbcnt;

	//bool longline = false; // to be used late for loops

	std::cout << "ZetaStack\n";
	std::string input;
	std::string bufferinput;
	std::string output;
	std::string finalOutput;
	std::string newline = ">>> ";
	std::string multiline = "... ";	

	while(true){
		std::cout << newline;
		std::getline(std::cin, input);

		input.push_back(' '); // prevent SIGSEGV by making sure len > 2, will get removed in preprocessor

		if(input[0] != '/'){

			// remove all whitespace and strip comments
		 	// Preprocessor.hpp
			input = comp::removeWhiteSpace(input);
			input = comp::stripcomment(input);

			if(input == ""){
				continue;
			}

			lbcnt = comp::checkleftBrac(input);
			rbcnt = comp::checkrightBrac(input);
			while(lbcnt != rbcnt/* || longline*/){
				std::cout << multiline;
				std::getline(std::cin, bufferinput);
				input.append(bufferinput);
				lbcnt = comp::checkleftBrac(input);
				rbcnt = comp::checkrightBrac(input);
			}


			/*
				-1 Error
				0 Calculate
				1 Assign
				2 Assign function
			*/
			runtype = comp::execType(input);
			if(cch::search(input) == "NULL"){

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
						finalOutput = asnExecuter(input, comp::assigntype(input));
						break;
					case 2:
						deffunction(input);
						break;
				}
				

			}else{
				finalOutput = cch::search(input);
				if(debug_mode){
					std::cout << "Cache Hit\n\n";
				}
			}
			if(do_exec && runtype != 2){		
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
