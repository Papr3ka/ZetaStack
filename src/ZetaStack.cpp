/*
  Copyright (C) 2020, Benjamin Yao
*/

#include<algorithm>
#include<atomic>
#include<chrono>
#include<exception>
#include<fstream>
#include<iomanip>
#include<iostream>
#include<ratio>
#include<string>
#include<thread>
#include<vector>

#if __cplusplus >= 201703L
	#include<filesystem> // Since C++17
#endif

#include<cassert>
#include<cctype>
#include<cmath>
#include<csignal>
#include<cstdio>
#include<cstdlib>
#include<ctime>

#include "Cache.hpp"
#include "Execute.hpp"
#include "Function.hpp"
#include "Preprocessor.hpp"
#include "Status.hpp"
#include "Token.hpp"
#include "Zetacompiler.hpp"

// Global Settings
static bool run = true;
static bool debug_mode = false; // Debugging 
static bool measure_time = false; // Measuring time
static bool do_exec = true; // Execute
static bool do_bar = true; // Loading bar
static bool do_sighandle = true; // Handle signals
static bool do_buffer = true;

// Default settings
/*--Variable------------Value--
*|  run               | true  |
*|  debug_mode        | false |
*|  measure_time      | false |
*|  do_exec           | true  |
*|  do_bar            | true  |
*|  do_sighandle      | true  |
*///---------------------------

// For Displaying the unit of time when "/time" is activated
const static std::string tunit[4] = {
	" ns",
	" µs",
	" ms",
	" s"
}; // len 4

// Works alongside tunit
// double becuase it is being compared with other doubles
const static double tinterval[4] = {
	1.0,
	1000.0,
	1000000.0,
	1000000000.0
};


/* Symbols
*  ⲗ 𝜁 ❯ 𝚭
*/

// Line that is printed in interface
const static std::string newline = "=== "; // ≡≡≡ is what windows terminal shows, looks like a stack hence the name 𝚭Stack
const static std::string multiline = "::: "; // When there is more of one bracket or quote

// For versioning
typedef struct{
	int major;
	int minor;
	int rev;

	bool special;
	int vertype;
	int specialrev;
}version;

// Current Version
const version curversion = {0, 2, 3, false, -1, -1};

// Version detect for compilers
#if defined(__clang__)
	const bool detect_comp = true;
	const version compilerversion = {__clang_major__,
                                     __clang_minor__,
                                     __clang_patchlevel__,
                                     false,
                                     -1,
                                     -1};
	const std::string compiler = "Clang";
#elif defined(__INTEL_COMPILER)
	const bool detect_comp = true;
	const std::string compiler = "ICC";
	const version compilerversion = {-1,-1,-1, false, -1, -1};
#elif defined(__GNUC__) || defined(__GNUG__)
	const bool detect_comp = true;
	const version compilerversion = {__GNUC__,
                                     __GNUC_MINOR__,
                                     __GNUC_PATCHLEVEL__,
                                     false,
                                     -1,
                                     -1};
	const std::string compiler = "GCC";
#elif defined(_MSC_VER)
	const bool detect_comp = true;
	const std::string compiler = "MSVC ".append(std::to_string(_MSC_VER));

#else
	const bool detect_comp = false;
    const version compilerversion = {-1,-1,-1, false, -1, -1};
    const std::string compiler = "Unknown";
#endif

std::string program_name;

// Detect OS
#ifdef _WIN32
    const std::string operatingsystem = "Windows 32-bit";
#elif _WIN64
    const std::string operatingsystem = "Windows 64-bit";
#elif __APPLE__ || __MACH__
    const std::string operatingsystem = "Mac OSX";
#elif __linux__
    const std::string operatingsystem = "Linux";
#elif __FreeBSD__
    const std::string operatingsystem = "FreeBSD";
#elif __unix || __unix__
    const std::string operatingsystem = "Unix";
#else
    const std::string operatingsystem = "";
#endif

// Print out a vector element by element
inline static void printvec(std::vector<std::string> print){
	std::cout << '[';
	for(unsigned long i=0; i < print.size(); i++){
		if(i >= 1){
			std::cout << ", ";
		}
   		std::cout << print.at(i);
	}
	std::cout << ']';
}

inline static void printvectoken(std::vector<token> print){
	std::cout << '[';
	for(unsigned long i=0; i < print.size(); i++){
		if(i >= 1){
			std::cout << ", ";
		}
   		std::cout << print.at(i).data; // .data
	}
	std::cout << ']';
}

// Flags
bool sigint_immune_flag = false;
bool sigcont_flag = false;

// Deal with version here [type: version -> type: string]
// Example 0.0.0-a.0
inline static std::string versioncomp(version ver){
	if(ver.major == -1){
		return "";
	}
	std::string output;
	output.append(std::to_string(ver.major));
	output.append(".");
	output.append(std::to_string(ver.minor));
	output.append(".");
	output.append(std::to_string(ver.rev));
	if(ver.special){
		switch(ver.vertype){
			case 0: // Alpha
				output.append("-a.");
				output.append(std::to_string(ver.specialrev));
				break;
			case 1: // Beta
				output.append("-b.");
				output.append(std::to_string(ver.specialrev));
				break;
			case 2: // Release candidate
				output.append("-rc.");
				output.append(std::to_string(ver.specialrev));
				break;
			default:
				return output;				
		}
	}
	return output;
}

// Activated when "/clock" is used. relies on sigint_immune_flag on when to exit
inline static void showclock(void){
	sigint_immune_flag = true;
	std::chrono::_V2::system_clock::time_point chrono_ctp;
	time_t tpointnow;
	char buffer[32];
	while(sigint_immune_flag){
		chrono_ctp = std::chrono::system_clock::now();
		tpointnow = std::chrono::system_clock::to_time_t(chrono_ctp);

		// Month DD YYYY HH:MM:SS:MIL
		std::strftime(buffer,sizeof(buffer),"%B %d %Y %T",std::localtime(&tpointnow));
		std::cout << "\r" << buffer << ":" << 
			std::chrono::duration_cast<std::chrono::milliseconds>(
			chrono_ctp.time_since_epoch()
			).count() % 1000 <<
			"\r";

		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::microseconds(500));
	}
	std::cout << "\r                                \r";
	return;
}

// commands using the "/" symbol
inline static void command(std::string com){
	if(com[0] == '/'){
		return;
	}else if(com[0] == ';'){
		int retval = std::system(com.substr(1,com.size()-1).c_str());
		if(retval != 0){
			std::cout << "Command processor not present\n";
		}
		return;
	}

	std::vector<std::string> cmdargv = split(com, " "); // split
	// index 0 is command. index > 0 is args for command or secondary command

	cmdargv.emplace_back(" "); // add space to make size > 1 in some cases to prevent SIGSEGV

	if(cmdargv.front() == "debug" || 
	   cmdargv.front() == "Debug" || 
	   cmdargv.front() == "dbg"){
		if(debug_mode){
			debug_mode = false;
			std::cout << "Debug off\n";
		}else{
			debug_mode = true;
			std::cout << "Debug on\n";
		}
	}else if(cmdargv.front() == "exit"){
		run = false;
		return;
	}else if(cmdargv.front() == "buffer"){
		if(cmdargv[1] == "clear"){
			var::clearbuffer();
			std::cout << "Buffer cleared\n";
		}else if(cmdargv[1] == "size"){
			unsigned long int rbsize = var::getrandbufsize();
			unsigned long int buffmax = var::getbuffermax();
			std::cout << rbsize << "/" << buffmax << " [" << strunc((double)rbsize / (double)buffmax * 100, 5) << "%]\n";
		}else if(cmdargv[1] == "setmax"){
			var::setbuffermax(std::stoull(cmdargv[2]));
		}else{
			std::cout << "Options for command \"" << 
                        cmdargv.front() << "\"\n   " << 
                        cmdargv.front() << " clear\n   " << 
                        cmdargv.front() << " setmax <max>\n   " <<
                        cmdargv.front() << " size\n";
		}
	}else if(cmdargv.front() == "Cache" || 
			 cmdargv.front() == "cache"){
		
		if(cmdargv[1] == "clear"){
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
		}else if(cmdargv[1] == "setmax"){
			cch::setmaxlen(std::stoull(cmdargv[2]));
		}else if(cmdargv[1] == "show"){
			std::vector<std::string> iden = cch::getiden();
			std::vector<std::string> val = cch::getval();
			std::cout << "Size: " << val.size() << "\n";
			if(val.size() >= 1) std::cout << "--------\n";
			for(unsigned long int idx=0; idx < iden.size(); idx++){
				std::cout << "\"" << iden[idx] << "\": \"" << val[idx] << "\"\n";
			}
		}else{
			std::cout << "Options for command \"" << 
                        cmdargv.front() << "\"\n   " << 
                        cmdargv.front() << " clear\n   " << 
                        cmdargv.front() << " toggle\n   " <<
                        cmdargv.front() << " setmax <max>\n   " <<
                        cmdargv.front() << " show\n";
		}
	}else if(cmdargv.front() == "del"){
		int delsuccess = 0;

		// For functions, delete all no mater the argument count
		if(cmdargv[1].back() == '('){
			delsuccess = udef(cmdargv[1]);
			if(delsuccess == 1){
				std::cout << "Undefined function: \"" << cmdargv[1] << "\"\n";
			}
			cch::refreshDepends(cmdargv[1]);
			cch::fulfill_depends();
		}else{
			std::vector<std::string> vgls = var::globals();
			for(auto x: vgls){
				if(x == cmdargv[1]){
					delsuccess += var::delvar(cmdargv[1]);
				}
			}
			if(delsuccess == 1){
				std::cout << "Undefined variable: \"" << cmdargv[1] << "\"\n";
			}else if(delsuccess == 2){
				std::cout << "Variable \"" << cmdargv[1] << "\" cannot be deleted\n";
			}
			cch::refreshDepends(cmdargv[1]);
		}
	}else if(cmdargv.front() == "globals"){
		unsigned long int startindex = 0;
		std::vector<std::string> sfunctions = getandassemble_all_defined_functions();
		std::cout << "Functions: " << sfunctions.size() << "\n";
		if(sfunctions.size() >= 1){
			std::sort(sfunctions.begin(), sfunctions.end(), [](std::string x, std::string y){return x < y;});
			std::cout << "--------\n";
			for(std::string prfunc: sfunctions){
				std::cout << prfunc << "\n";
			}
			std::cout << std::endl;
		}
		std::vector<std::string> globalvars = var::globals();
		startindex = var::specials().size() - 1;
		subVec(globalvars, startindex, globalvars.size());
		std::cout << "Variables: " << globalvars.size() << "\n";
		startindex = 0;
		if(globalvars.size() >= 1){
			std::sort(globalvars.begin(), globalvars.end(), [](std::string x, std::string y){return x < y;});
			std::cout << "--------\n";
			for(;startindex < globalvars.size(); startindex++){
				std::cout << "\"" << globalvars[startindex] << "\": " << var::search(globalvars[startindex]) << "\n";
			}		
		}
	}else if(cmdargv.front() == "time"){
		if(measure_time){
			measure_time = false;
			std::cout << "Time off\n";
		}else{
			measure_time = true;
			std::cout << "Time on\n";

		}
	}else if(cmdargv.front() == "clock"){
		showclock();
		return;
	}else if(cmdargv.front() == "exec"){
		if(do_exec){
			do_exec = false;
			std::cout << "Execute off\n";
		}else{
			do_exec = true;
			std::cout << "Execute on\n";
		}
	}else if(cmdargv.front() == "export"){
		if(cmdargv.size() >= 3){
			std::string writedata;
			if(var::exists(cmdargv[1])){
				unsigned long long int filesize;

				// Get data that needs to be written to txt file
				writedata = var::search(cmdargv[1]);
				std::string filename = cmdargv[1];
				filename.append(".txt");

				// Create 2 separate streams
				std::ofstream writefile;

				// To verify file is empty
				std::ifstream checkfile;

				checkfile.open(filename);
				// Check if empty
			 	if(checkfile.peek() == std::ifstream::traits_type::eof()){
			 		writefile.open(filename);
			 		writefile << writedata;
			 		writefile.close();

			 		// <filesystem> was introduced C++17
		 			#if __cplusplus >= 201703L
		 				// Actually get size of file
		 				std::filesystem::path getfisze{filename};
		 				filesize = std::filesystem::file_size(getfisze.u8string());
		 			#else
		 				// Get size of string not file
		 				// size of string == size of file
		 				filesize = (sizeof(writedata)*writedata.size())/8;
		 			#endif 	

			 		std::cout << "Successfully wrote to \"" << filename << "\" (size: " << filesize << "B)\n";
			 		checkfile.close();				 		
			 		return;
			 	}else{
			 		std::string confirm;
			 		std::cout << "Confirm overwrite file \"" << filename << "\" [Y/N]? ";
			 		std::getline(std::cin, confirm);

			 		// Detect failstate
			 		if(std::cin.fail()){
						std::cin.clear();
						std::cout << std::endl;
						return;
					}

					// change all input to uppercase 
			 		std::transform(confirm.begin(), confirm.end(),confirm.begin(), ::toupper);
			 		if(confirm == "Y" || confirm == "YE" || confirm == "YES"){

			 			writefile.open(filename, std::ofstream::out | std::ofstream::trunc);
			 			writefile << writedata;
			 			writefile.close();

			 			#if __cplusplus >= 201703L
			 				std::filesystem::path getfisze{filename};
			 				filesize = std::filesystem::file_size(getfisze.u8string());
			 			#else
			 				filesize = (sizeof(writedata)*writedata.size())/8;
			 			#endif 
			 			std::cout << "Successfully wrote to \"" << filename << "\" (size: " << filesize << "B)\n";
			 			checkfile.close();
			 		}else{
			 			checkfile.close();
			 			return;
			 		}
			 	}
			}else{
				std::cout << "Undefined variable: \"" << cmdargv[1] << "\"\n";
			}

		}
	}else if(cmdargv.front() == "bar"){
		if(do_bar){
			do_bar = false;
			bar::setstate(false);
			std::cout << "Progress Bar off\n";
		}else{
			do_bar = true;
			std::cout << "Progress Bar on\n";
		}
	}else if(cmdargv.front() == "help"){
		// Commands accessed via the '/' key
		std::cout << "   bar                      Toggles progress bar \n"
                  << "   buffer <Options>         Commands related to buffer\n"
                  << "   cache <Options>          Commands related to cache\n"
                  << "   clock                    Displays a clock\n"
                  << "   debug                    Toggles debug mode\n"
                  << "   del <var>                Deletes a variable\n"
                  << "   exec                     Toggles execution\n"
                  << "   exit                     Exits the program\n"
                  << "   export <var>             Writes a variable to text\n"
                  << "   time                     Times calculations\n"
                  << "   ; <Command>              Executes system command\n";
	}else{

		// Command not found
		std::cout << "Unkown Command: " << com << "\n";
	}
}

// Changes settings according to command-line arguments
// Must be run called first when program starts
inline static void arghandler(std::vector<std::string> args){
	unsigned long int arg_index = 1;

	// Change program_name to the name of exe
	std::size_t frfound = args.front().rfind("/");
	std::size_t brfound = args.front().rfind("\\");
	std::size_t extfound = args.front().rfind(".");
	if(extfound != std::string::npos){
		args.front() = args.front().substr(0, static_cast<int>(extfound));
	}
	if(frfound == std::string::npos && brfound == std::string::npos){
		program_name = args.front();
	}else{
		if(brfound == std::string::npos){
			program_name = args.front().substr(static_cast<int>(frfound) + 1,args.front().size());
		}else{
			program_name = args.front().substr(static_cast<int>(brfound) + 1,args.front().size());
		}
	}


	while(arg_index < args.size()){
		if(args.at(arg_index) == "--version" || args.at(arg_index) == "-v"){
			std::cout << program_name << " Version " << versioncomp(curversion) << "\n";
			if(detect_comp){
				std::cout << "  (Built with " << compiler << " " << versioncomp(compilerversion);
				#if defined(__DATE__) && defined(__TIME__)
					std::cout << ", " << __DATE__ << ", " << __TIME__ << ")";
				#else
					std::cout << ")";
				#endif
				if(operatingsystem != ""){
					std::cout << " on " << operatingsystem << "\n";
				}else{
					std::cout << "\n";
				}
			}
			run = false;
			do_sighandle = false;
			do_buffer = false;
			return;
		}else if(args.at(arg_index) == "--debug"){
			debug_mode = true;
		}else if(args.at(arg_index) == "--nobuffer"){
			do_buffer = false;
		}else if(args.at(arg_index) == "--noexec"){
			do_exec = false;
		}else if(args.at(arg_index) == "--nohandle"){
			do_sighandle = false;
		}else if(args.at(arg_index) == "--maxrecurse"){
			if(arg_index + 1 < args.size()){
				try{
					xmath::setmaxrecurse(std::stoull(args.at(arg_index + 1))); // used in Execute.hpp && Execute.cpp
				}catch(const std::exception&){
					// Error
				}
			}
			arg_index++;			
		}else if(args.at(arg_index) == "--help" || args.at(arg_index) == "-h"){
			std::cout << "Usage: " << program_name << " [Options] ...\n\n"
                      << "Options:\n"
                      << "  -h  --help                   Display this information\n"
                      << "  -v  --version                Display version information\n\n"
                      // Intentional Space
                      << "  --debug                      Start with debug mode on\n"
                      << "  --nobuffer                   Disables variable buffer\n"
                      << "  --noexec                     Start with execution disabled\n"
                      << "  --nohandle                   Disables signal handling\n"
                      << "  --maxrecurse <int>           Sets the maximum recursion depth\n";

			run = false;
			do_sighandle = false;
			do_buffer = false;
			return;
		}else{
			// Unknown argument
			std::cout << "Ambiguous command-line option: \""<< args.at(arg_index) << "\"\n";
			run = false;
			do_sighandle = false;
			do_buffer = false;
			return;			
		}	
		arg_index++;
	}
	std::cout << program_name << " " << versioncomp(curversion) <<"\n";
	return;
}


/*-Interpreting process order-----------------------------------------------Resposible Files--------------------
*|  1. Parser "2*(14+12)" -> [2,*,(,14,+,12,)]                          |  Preprocessor.hpp / Preprocessor.cpp |
*|  2. Lexer [2,MUL,L_BRAC,14,ADD,12,R_BRAC]                            |  Preprocessor.hpp / Preprocessor.cpp |
*|  3. RPN [2, 14, 12, ADD, MUL]                                        |  Zetacompiler.hpp / Zetacompiler.cpp |
*|  4. Execute                                                          |  Execute.hpp / Execute.cpp           |
*///------------------------------------------------------------------------------------------------------------

// Directly executing statements
inline static std::string evaluate(const std::string& input){
	unsigned long int tscale;
	double totaltime;

	if(do_bar) bar::start();
	bar::setstate(do_bar);

	std::chrono::_V2::high_resolution_clock::time_point lex_tstart;
	std::chrono::_V2::high_resolution_clock::time_point lex_tend;
	std::chrono::_V2::high_resolution_clock::time_point tcomp_tstart;
	std::chrono::_V2::high_resolution_clock::time_point tcomp_tend;
	std::chrono::_V2::high_resolution_clock::time_point shyd_tstart;
	std::chrono::_V2::high_resolution_clock::time_point shyd_tend;
	std::chrono::_V2::high_resolution_clock::time_point varfill_tstart;
	std::chrono::_V2::high_resolution_clock::time_point varfill_tend;
	std::chrono::_V2::high_resolution_clock::time_point exec_tstart;
	std::chrono::_V2::high_resolution_clock::time_point exec_tend;
	
	std::vector<token> output;
	std::vector<std::string> s_out;
	std::string finalOutput;

	std::vector<std::string> non_const;

	bar::inform("Parsing");
	lex_tstart = std::chrono::high_resolution_clock::now();	
	s_out = comp::lex(input);	
	lex_tend = std::chrono::high_resolution_clock::now();

	if(debug_mode){
		bar::setstate(false);
		std::cout << "Parser:             ";
		printvec(s_out);
		std::cout << "\n";
		bar::setstate(true);
	}

	if(s_out.size() <= 0){
		bar::setstate(false);
		return "";
	} 

	output.reserve(s_out.size());

	bar::inform("Lexing");
	tcomp_tstart = std::chrono::high_resolution_clock::now();
	output = comp::tokenComp(s_out);
	tcomp_tend = std::chrono::high_resolution_clock::now();
	if(debug_mode){
		bar::setstate(false);
		std::cout << "Lexer:              ";
		printvectoken(output);
		std::cout << "\n";
		bar::setstate(true);
	}

	// Free up memory
	std::vector<std::string>().swap(s_out);

	for(token dep: output){
		if(dep.type == 4 || dep.type == 5){
			non_const.emplace_back(dep.data);
		}
	}

	if(cch::search(input) != "NULL"){
		bar::setstate(false);
		if(debug_mode){
			std::cout << "Cache Hit\n\n";
		}
		return cch::search(input);
	}
	
	try{
		bar::inform("Filling Variables");
		varfill_tstart = std::chrono::high_resolution_clock::now();
		output = comp::fillallvars(output); // Zetacompiler.hpp
		varfill_tend = std::chrono::high_resolution_clock::now();
	}catch(const std::string& error){
		bar::setstate(false);
		if(do_bar){
			bar::stop();
			bar::finish(); // print out <CR> and whitespace			
		}
		return error;
	}

	if(output.size() <= 0){
		bar::setstate(false);
		return "";
	}

	bar::inform("RPN");

	comp_metadata rpn_metadata = comp::getcompdata(output);

	shyd_tstart = std::chrono::high_resolution_clock::now();
	output = comp::shuntingYard(output, rpn_metadata); // Zetacompiler.hpp
	shyd_tend = std::chrono::high_resolution_clock::now();					
	if(debug_mode){
		bar::setstate(false);
		std::cout << "RPN:                ";
		printvectoken(output);
		std::cout << "\n\n";
		bar::setstate(true);
	}
	
	if(output.size() <= 0){
		bar::setstate(false);
		return "";
	} 

	if(do_exec){
		try{
			bar::changemode(1);
			bar::init((long int)output.size());
			bar::inform("Executing");
			exec_tstart = std::chrono::high_resolution_clock::now();
			finalOutput = xmath::calculate(output, do_bar, rpn_metadata.nums);
			exec_tend = std::chrono::high_resolution_clock::now();
		}catch(const std::string& error){
			xmath::resetsstreamsettings();
			bar::setstate(false);
			if(do_bar){
				bar::stop();
				bar::finish(); // print out <CR> and whitespace
			}
			return error;	
		}catch(const std::invalid_argument& err){
			xmath::resetsstreamsettings();
			bar::setstate(false);
			if(do_bar){
				bar::stop();
				bar::finish(); // print out <CR> and whitespace
			}			
			return "Unexpected Token";
		}catch(const std::exception &err){
			xmath::resetsstreamsettings();
			bar::setstate(false);
			if(do_bar){
				bar::stop();
				bar::finish(); // print out <CR> and whitespace
			}
			std::string outerr = "Error: ";
			outerr.append(err.what());
			return outerr;		
		}
		
		cch::update(input, finalOutput, non_const);

	}else{
		exec_tstart = std::chrono::high_resolution_clock::now();
		exec_tend = exec_tstart;
	}
	bar::setstate(false);
	if(do_bar){
		bar::stop();
		bar::finish(); // print out <CR> and whitespace
	}	
	xmath::resetsstreamsettings();
	
	if(measure_time){
		double lextime = std::chrono::duration<double, std::nano>(lex_tend - lex_tstart).count();
		double tcomptime = std::chrono::duration<double, std::nano>(tcomp_tend - tcomp_tstart).count();
		double shydtime = std::chrono::duration<double, std::nano>(shyd_tend - shyd_tstart).count();
		double varfilltime = std::chrono::duration<double, std::nano>(varfill_tend - varfill_tstart).count();
		double exectime = std::chrono::duration<double, std::nano>(exec_tend - exec_tstart).count();
		totaltime = lextime+tcomptime+shydtime+varfilltime+exectime;

		if(totaltime < 1000){
			tscale = 0;
		}else if(totaltime >= 1000 && totaltime < 1000000){
			tscale = 1;
			lextime /= 1000;
			tcomptime /= 1000;
			shydtime /= 1000;
			varfilltime /= 1000;
			exectime /= 1000;
			totaltime /= 1000;
		}else if(totaltime >= 1000000 && totaltime < 1000000000){
			tscale = 2;
			lextime /= 1000000;
			tcomptime /= 1000000;
			shydtime /= 1000000;
			varfilltime /= 1000000;
			exectime /= 1000000;
			totaltime /= 1000000;
		}else{
			tscale = 3;
			lextime /= 1000000000;
			tcomptime /= 1000000000;
			shydtime /= 1000000000;
			varfilltime /= 1000000000;
			exectime /= 1000000000;
			totaltime /= 1000000000;
		}		
		std::cout << "Time variable:\n   Parse:              " << lextime << 
				 tunit[tscale] << " \n   Lexer:              " << tcomptime  <<
				 tunit[tscale] << " \n   RPN:                " << shydtime <<
				 tunit[tscale] << " \n   Variable Filler:    " << varfilltime <<
				 tunit[tscale] << " \n   Execution:          " << exectime <<
				 tunit[tscale] << " \n   Total:              " << totaltime <<
				 tunit[tscale] << " \n\n";
	}
	cch::fulfill_depends();
	var::update("ans",finalOutput);
	return finalOutput;
}


// Define a function
inline static void deffunction(const std::string& input){
	unsigned long int tscale;
	double totaltime;

	if(do_bar) bar::start(); // Start the loading bar if option is true
	bar::setstate(do_bar);

	// Declare time points
	std::chrono::_V2::high_resolution_clock::time_point lex_tstart;
	std::chrono::_V2::high_resolution_clock::time_point lex_tend;
	std::chrono::_V2::high_resolution_clock::time_point tcomp_tstart;
	std::chrono::_V2::high_resolution_clock::time_point tcomp_tend;
	std::chrono::_V2::high_resolution_clock::time_point shyd_tstart;
	std::chrono::_V2::high_resolution_clock::time_point shyd_tend;
	std::chrono::_V2::high_resolution_clock::time_point decl_tstart;
	std::chrono::_V2::high_resolution_clock::time_point decl_tend;

	std::vector<std::string> partasn = comp::spliteq(input);
	partasn.back() = comp::removeWhiteSpace(partasn.back());

	bar::inform("Parsing");
	lex_tstart = std::chrono::high_resolution_clock::now();
	std::vector<std::string> sfunchead = comp::lex(partasn.front());
	std::vector<std::string> sfuncbody = comp::lex(partasn.back());
	lex_tend = std::chrono::high_resolution_clock::now();
	if(debug_mode){
		bar::setstate(false);
		std::cout << "Parser:             [";
		for(std::string x: sfunchead){
			std::cout << x << ", ";
		}
		std::cout << "=, ";
		for(unsigned long int x = 0; x < sfuncbody.size(); x++){

			std::cout << sfuncbody[x];
			if(x + 1 < sfuncbody.size()){
				std::cout << ", ";
			}
		}
		std::cout << "]\n";
		bar::setstate(true);
	}
	bar::inform("Lexing");
	tcomp_tstart = std::chrono::high_resolution_clock::now();
	std::vector<token> funchead = comp::tokenComp(sfunchead);
	std::vector<token> funcbody = comp::tokenComp(sfuncbody);
	tcomp_tend = std::chrono::high_resolution_clock::now();

	// Free up memory
	std::vector<std::string>().swap(sfunchead);
	std::vector<std::string>().swap(sfuncbody);

	if(debug_mode){
		bar::setstate(false);
		std::cout << "Lexer:              [";
		for(token x: funchead){
			std::cout << x.data << ", ";
		}
		std::cout << "=, ";
		for(unsigned long int x = 0; x < funcbody.size(); x++){

			std::cout << funcbody[x].data;
			if(x + 1 < funcbody.size()){
				std::cout << ", ";
			}
		}
		std::cout << "]\n";
		bar::setstate(true);
	}
	bar::inform("RPN");

	comp_metadata rpn_metadata = comp::getcompdata(funcbody);

	shyd_tstart = std::chrono::high_resolution_clock::now();
	funcbody = comp::shuntingYard(funcbody, rpn_metadata);
	shyd_tend = std::chrono::high_resolution_clock::now();
	if(debug_mode){
		bar::setstate(false);
		std::cout << "RPN:                [";
		for(unsigned long int x = 0; x < funcbody.size(); x++){
			std::cout << funcbody[x].data;
			if(x + 1 < funcbody.size()){
				std::cout << ", ";
			}
		}
		std::cout << "]\n";
		bar::setstate(true);
	}
	unsigned long int sepidx = 0;
	while(sepidx < funchead.size()){
		if(funchead.at(sepidx).type == 7){
			funchead.erase(funchead.begin()+sepidx);
			sepidx--;
		}
		sepidx++;
	}
	decl_tstart = std::chrono::high_resolution_clock::now();
	def(funchead, funcbody);
	decl_tend = std::chrono::high_resolution_clock::now();

	cch::refreshDepends(funchead.front().data);
	cch::fulfill_depends();

	bar::setstate(false);
	if(do_bar){
		bar::stop();
		bar::finish(); // print out <CR> and whitespace			
	}

	// Calculate time taken by each step
	if(measure_time){
		double lextime = std::chrono::duration<double, std::nano>(lex_tend - lex_tstart).count();
		double tcomptime = std::chrono::duration<double, std::nano>(tcomp_tend - tcomp_tstart).count();
		double shydtime = std::chrono::duration<double, std::nano>(shyd_tend - shyd_tstart).count();
		double decltime = std::chrono::duration<double, std::nano>(decl_tend - decl_tstart).count();
		totaltime = lextime+tcomptime+shydtime+decltime;
		if(totaltime < 1000){
			tscale = 0;
		}else if(totaltime >= 1000 && totaltime < 1000000){
			tscale = 1;
			lextime /= 1000;
			tcomptime /= 1000;
			shydtime /= 1000;
			totaltime /= 1000;
			decltime /= 1000;
		}else if(totaltime >= 1000000 && totaltime < 1000000000){
			tscale = 2;
			lextime /= 1000000;
			tcomptime /= 1000000;
			shydtime /= 1000000;
			totaltime /= 1000000;
			decltime /= 1000000;
		}else{
			tscale = 3;
			lextime /= 1000000000;
			tcomptime /= 1000000000;
			shydtime /= 1000000000;
			totaltime /= 1000000000;
			decltime /= 1000000000;
		}
		std::cout << "Time variable:\n   Parse:              " << lextime << 
				 tunit[tscale] << " \n   Lexer:              " << tcomptime  <<
				 tunit[tscale] << " \n   RPN:                " << shydtime <<
				 tunit[tscale] << " \n   Declare:            " << decltime <<
				 tunit[tscale] << " \n   Total:              " << totaltime <<
				 tunit[tscale] << " \n\n";
	}
	return;
}

// Signal Handler
inline static void sighandle(int sigtype){
	switch(sigtype){
		case SIGINT:
			bar::setstate(false);
			if(sigint_immune_flag){
				sigint_immune_flag = false;
				return;
			}else{
				if(do_bar){
					bar::stop();
					bar::finish(); // print out <CR> and whitespace
				}				
				std::cerr << "\nSignal (" << sigtype << ")\n";
				exit(0);	
			}

		//case SIGABRT:
		//	return;

		// Handles commands / child process
		case SIGCHLD:
			return;

		// SIGCONT
		case 18:
			if(sigcont_flag){
				sigcont_flag = false;
			}
			return;

		// Handles resizing of windows
		case SIGWINCH:
			return;
		default:
			bar::setstate(false);
			if(do_bar){
				bar::stop();
				bar::finish(); // print out <CR> and whitespace
			}		
			std::cerr << "\nSignal (" << sigtype << ")\n";
			exit(0); 
	}
}

// Main entry point
int main(int argc, char* argv[]){

	std::vector<std::string> str_argv(argv, argv+argc);
	arghandler(str_argv);

	// Handle every signal
	if(do_sighandle){
		for(int signal = 1; signal <= 64; signal++){
			std::signal(signal, sighandle);
		}
	}

	// Separate threads to run loading bar and variable buffer
	std::thread progress(bar::barmanager);
	std::thread buffer(var::buffer, do_buffer);

	if(!do_buffer){
		var::setbuffermax(0);
		var::joinbuffer();
		buffer.join();
	}
	
	// Decides whether a function is being declared or statement being evaluated
	int runtype;

	// Checks the left and right bracket count
	int lbcnt;
	int rbcnt;

	// Checks if the quotes are mismatched
	bool evenquote;

	std::string input;
	std::string bufferinput;
	std::string output;
	std::string finalOutput;

	mainloopstart:
	while(run){

		std::cout << newline;
		std::getline(std::cin, input);

		// Check if CTRL+Z or CIN is in fail state and clear it
		if(std::cin.fail()){
			std::cin.clear();
			std::cout << std::endl;
			goto mainloopstart;
		}
		
		if(input[0] != '/'){

			// remove all whitespace and strip comments
		 	// Preprocessor.hpp
		 	input = comp::stripcomment(input);
			input = comp::removeWhiteSpace(input);

			if(input == ""){
				continue;
			}

			lbcnt = comp::checkleftBrac(input);
			rbcnt = comp::checkrightBrac(input);
			evenquote = comp::quotecount(input);

			while(lbcnt > rbcnt || evenquote){
				sigcont_flag = true; // Sets a global flag that is changed by signal handler

				std::cout << multiline;
				std::getline(std::cin, bufferinput);

				// Check if CTRL+Z or CIN is in fail state and clear it
				if(std::cin.fail() || !sigcont_flag){
					std::cin.clear();
					std::cout << std::endl;
					goto mainloopstart;
				}
				input.append(bufferinput);

				// Recheck brackets and quotes
				lbcnt = comp::checkleftBrac(input);
				rbcnt = comp::checkrightBrac(input);
				evenquote = comp::quotecount(input);
			}
			/*
				X -1 Error - depracated pre-alpha
				0 Calculate
				X 1 Assign - depracated v 0.1.0
				2 Assign function
			*/
			runtype = comp::execType(input);
			switch(runtype){
				case 0:
					finalOutput = evaluate(input);						
					break;
				case 2:
					deffunction(input);
					break;
				default:
					goto mainloopstart;
			}
				
			if(do_exec && runtype != 2 && finalOutput.size() >= 1){
				std::cout << finalOutput << "\n";
			}
		}else{
			// commands
			input.erase(input.begin());
			command(input);
		}
	}
	bar::join();
	progress.join();
	if(do_buffer){
		var::joinbuffer();
		buffer.join();
	}
	return 0;
}