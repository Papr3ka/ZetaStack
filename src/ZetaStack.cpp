/*
  Copyright (C) 2020 - 2021, Benjamin Yao
*/

#include<algorithm>
#include<atomic>
#include<array>
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

#include "Analyzer.hpp"
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
static bool bare = false;

// Default settings
/*--Variable------------Value--
*|  run               | true  |
*|  debug_mode        | false |
*|  measure_time      | false |
*|  do_exec           | true  |
*|  do_bar            | true  |
*|  do_sighandle      | true  |
*|  do_buffer         | true  |
*|  bare              | false |
*///---------------------------

// For Displaying the unit of time when "/time" is activated
const static std::array<std::string, 4> tunit{
	" ns",
	" µs",
	" ms",
	" s"
}; // len 4

/* Symbols
*  ⲗ 𝜁 ❯ 𝚭
*/


unsigned long int CPU_COUNT = std::thread::hardware_concurrency();

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
const version curversion = {0, 3, 0, false, -1, -1};

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
#elif defined(__INTEL_COMPILER) || defined(__ICL)
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
	const std::string compiler = "MSVC";
	const version compilerversion = {-1,-1,-1, false, -1, -1};
#else
	const bool detect_comp = false;
    const version compilerversion = {-1,-1,-1, false, -1, -1};
    const std::string compiler = "Unknown";
#endif

std::string program_name;

// Detect OS
#if defined(_WIN16) || defined(_WIN32) || \
    defined(__WIN32__) || defined(_WIN64) || \
    defined(_WIN32_WCE) || defined(__TOS_WIN__) || \
    defined(__WINDOWS__)
	const std::string operatingsystem = "Windows";
#elif defined(_AIX) || defined(__TOS_AIX__)
    const std::string operatingsystem = "AIX";
#elif defined(__APPLE__) || defined(__MACH__)
    const std::string operatingsystem = "Mac OSX";
#elif defined(__linux__)
    const std::string operatingsystem = "Linux";
#elif defined(__FreeBSD__) 
    const std::string operatingsystem = "FreeBSD";
#elif defined(__NetBSD__)
    const std::string operatingsystem = "NetBSD";
#elif defined(__OpenBSD__)
    const std::string operatingsystem = "OpenBSD";
#elif defined(__DragonFly__)
    const std::string operatingsystem = "DragonFly";
#elif defined(__CYGWIN__)
    const std::string operatingsystem = "Cygwin";
#elif defined(sun) || defined(__sun)
    const std::string operatingsystem = "Solaris";    
#elif defined(__unix) || defined(__unix__)
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
bool inturrupt_exit_flag = false;
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

		// Extra Spaces are intentional
		// Month DD YYYY HH:MM:SS:MIL
		std::strftime(buffer,sizeof(buffer),"%B %d %Y %T      ",std::localtime(&tpointnow));
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
inline static void command(const std::string& com){
	if(com[0] == '/'){
		return;
	}
	std::size_t shcmd_pre = com.rfind(";");
	if(shcmd_pre != std::string::npos){

		unsigned long int shcmd_pre_pos = static_cast<unsigned long int>(shcmd_pre);
		std::string metacom = com.substr(0, shcmd_pre_pos);
		std::chrono::_V2::steady_clock::time_point tstart;
		std::chrono::_V2::steady_clock::time_point tend;
		if(metacom == "t" || metacom == "time"){
			tstart = std::chrono::steady_clock::now();
		}

		int retval = std::system(com.substr(shcmd_pre_pos + 1,com.size()-shcmd_pre_pos).c_str());

		if(metacom != "q" && metacom != "quiet"){
			std::cout << "\n\"" << com.substr(shcmd_pre_pos + 1,com.size()-shcmd_pre_pos) << "\" returned " << retval << "\n";
		}

		if(metacom == "t" || metacom == "time"){
			tend = std::chrono::steady_clock::now();
			std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << " ms\n";
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
			unsigned long long int setval = std::stoull(cmdargv[2]);
			unsigned long long int memreq = setval*sizeof(std::vector<long int>)/3145728;
			if(memreq >= 512){
				std::cout << "Warning: setting buffer to size " << setval << " requires " << memreq << " MB\n";
				std::cout << "Proceed? [Y/N] ";
				std::string confirm;
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
		 			var::setbuffermax(setval);
		 		}
			}else if(memreq >= 64){
				std::cout << "Warning: setting buffer to size " << setval << " requires " << memreq << " MB\n";
				var::setbuffermax(setval);
			}else{
				var::setbuffermax(setval);
			}
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
			cch::setmaxlen(std::stoul(cmdargv[2]));
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
			}else if(delsuccess == 2){
				std::cout << "Function \"" << cmdargv[1] << "\" Cannot be deleted\n";
			}else{
				cch::refreshDepends(cmdargv[1]);
				cch::fulfill_depends();	
			}
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
		std::vector<std::string> sfunctions;
		std::vector<std::string> builtinvars = var::specials();
		std::sort(builtinvars.begin(), builtinvars.end(), [](std::string x, std::string y){return x < y;});
		if(cmdargv.size() >= 3 && cmdargv[1] == "builtin"){
			sfunctions = getandassemble_all_builtin_functions();
			std::cout << "Built-in functions: " << sfunctions.size() << "\n";
			if(sfunctions.size() >= 1){
				//std::sort(sfunctions.begin(), sfunctions.end(), [](std::string x, std::string y){return x < y;});
				std::cout << "-------------------\n";
				for(std::string prfunc: sfunctions){
					std::cout << prfunc << "\n";
				}
				std::cout << std::endl;
			}
			
			std::cout << "Built-in variables: " << builtinvars.size() << "\n";
			if(builtinvars.size() >= 1){
				std::cout << "-------------------\n";
				for(std::string bvar: builtinvars){
					std::cout << bvar << "\n";
				}
				std::cout << std::endl;
			}
		}else{
			sfunctions = getandassemble_all_defined_functions();
			std::cout << "Functions: " << sfunctions.size() << "\n";
			if(sfunctions.size() >= 1){
				std::sort(sfunctions.begin(), sfunctions.end(), [](std::string x, std::string y){return x < y;});
				std::cout << "----------\n";
				for(std::string prfunc: sfunctions){
					std::cout << prfunc << "\n";
				}
				std::cout << std::endl;
			}
			std::vector<std::string> globalvars = var::globals();
			startindex = builtinvars.size();
			startindex--;
			subVec(globalvars, startindex, globalvars.size());
			std::cout << "Variables: " << globalvars.size() << "\n";
			startindex = 0;
			if(globalvars.size() >= 1){
				std::sort(globalvars.begin(), globalvars.end(), [](std::string x, std::string y){return x < y;});
				std::cout << "----------\n";
				for(;startindex < globalvars.size(); startindex++){
					std::cout << "\"" << globalvars[startindex] << "\": " << var::search(globalvars[startindex]) << "\n";
				}
				std::cout << std::endl;		
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
	}else if(cmdargv.front() == "importvar"){
	
		if(cmdargv.size() >= 3){
			std::string filename = cmdargv[1];
			std::string vartoname = cmdargv[1];
			if(filename.size() >= 4){
				std::size_t findext = filename.rfind(".");
				if(findext != std::string::npos){
					vartoname = vartoname.substr(0, static_cast<long int>(findext));
				}
				if(!var::changable(vartoname)){
					std::cout << "Variable \"" << vartoname << "\" is read-only\n";
					return;
				}
			}
			std::ifstream checkfile(filename);
			if(checkfile.good()){
				std::stringstream importbuffer;
				importbuffer << checkfile.rdbuf();
				try{
					std::stod(importbuffer.str());
					var::update(vartoname, importbuffer.str());
				}catch(const std::invalid_argument&){
					std::cout << "Invalid file format\n";
				}
				
			}else{
				std::cout << "File \"" << filename << "\" not found\n";
			}
			checkfile.close();
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
                  << "   importvar <file>         Imports a variable from a file\n"
                  << "   time                     Times calculations\n"
                  << "   globals                  Displays all defined functions and variables\n"
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

	// Change program_name to the name of exe without ext
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

	std::string currentarg;
	std::size_t splitfind;
	long long int argnum = -1;
	while(arg_index < args.size()){
		splitfind = args[arg_index].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_/! ");
		if(splitfind == std::string::npos){
			currentarg = args[arg_index];
			argnum = -1;
		}else{
			currentarg = args[arg_index].substr(0, static_cast<unsigned long int>(splitfind));
			try{
				argnum = std::stoll(args[arg_index].substr(static_cast<unsigned long int>(splitfind),
                                                           args[arg_index].size() - static_cast<unsigned long int>(splitfind)));			
			}catch(const std::exception&){
				// Error
				argnum = -1;
			}
		}
		if(currentarg == "--version" || currentarg == "-v"){

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
			exit(0);
		}else if(currentarg == "-j"){
			if(argnum != -1){
				CPU_COUNT = (unsigned long int)argnum;
			}else{
				try{
					arg_index++;
					CPU_COUNT = std::stoul(args.at(arg_index));
				}catch(const std::exception&){
					// error
				}	
			}
			switch(CPU_COUNT){
				case 0:
					CPU_COUNT = std::thread::hardware_concurrency();
					break;
				case 1:
					do_bar = false;
					do_buffer = false;
					break;
				case 2:
					do_bar = false;
					break;
				default:
					break;
			}
		}else if(currentarg == "--bare"){
			bare = true;
			do_buffer = false;
			do_bar = false;
			cch::setstate(false);
			cch::setmaxlen(1);
			var::setbuffermax(1);
			xmath::setmaxrecurse(512);
		}else if(currentarg == "--debug"){
			debug_mode = true;
		}else if(currentarg == "--nobuffer"){
			do_buffer = false;
		}else if(currentarg == "--noexec"){
			do_exec = false;
		}else if(currentarg == "--nohandle"){
			do_sighandle = false;
		}else if(currentarg == "--maxrecurse"){
			if(argnum != -1){
				xmath::setmaxrecurse((unsigned long long int)argnum); // used in Execute.hpp && Execute.cpp
			}else{
				try{
					arg_index++;
					xmath::setmaxrecurse(std::stoull(args.at(arg_index)));		
				}catch(const std::exception&){
					// error
				}
			}	
		}else if(currentarg == "--help" || args.at(arg_index) == "-h"){
			std::cout << "Usage: " << program_name << " [Options] ...\n\n"
                      << "Options:\n"
                      << "  -h  --help                   Display this information\n"
                      << "  -v  --version                Display version information\n\n"
                      // Intentional Space
                      << "  -j <int>                     Sets the maximum thread count\n\n"
                      // Intentional Space
                      << "  --bare                       Start with absolute minimum memory usage\n"
                      << "  --debug                      Start with debug mode on\n"
                      << "  --nobuffer                   Disables variable buffer\n"
                      << "  --noexec                     Start with execution disabled\n"
                      << "  --nohandle                   Disables signal handling\n"
                      << "  --maxrecurse <int>           Sets the maximum recursion depth\n"; // Not good idea to increase
			exit(0);
		}else{
			// Unknown argument
			std::cout << "Ambiguous command-line option: \""<< currentarg << "\"\n";
			exit(1);
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

/* Note: When returning from 

	std::string evaluate(const std::string&) 
	void deffunction(const std::string&)

Always call bar::setstate(false)

*/
// Directly executing statements
inline static std::string evaluate(const std::string& input){
	unsigned long int tscale;
	double totaltime;
	if(do_bar) bar::start();
	bar::setstate(do_bar);

	inturrupt_exit_flag = true;

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
		if(!checkrpn(output)){
			bar::setstate(false);
			return "Unexpected Token";
		}
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

	inturrupt_exit_flag = true;

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

	std::vector<token> variables;
	variables.reserve(funchead.size());


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

	for(token chkvar: funcbody){
		if(chkvar.type == 5){
			variables.emplace_back(chkvar);
		}
	}

	if(f_isspecial(funchead.front().data, funchead.size() - 3)){
		bar::setstate(false);
		std::cout << "Cannot redeclare function \"" << funchead.front().data << "\"\n";
		return;
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

	if(!checkrpn(funcbody)){
		bar::setstate(false);
		std::cout << "Unexpected Token\n";
		return;
	}	

	try{
		decl_tstart = std::chrono::high_resolution_clock::now();
		funcbody = xmath::simplify(funcbody, variables, funchead.front().data);
		def(funchead, funcbody);
		decl_tend = std::chrono::high_resolution_clock::now();

		cch::refreshDepends(funchead.front().data);
		cch::fulfill_depends();
	}catch(const std::string& error){
		if(error == ""){
			bar::setstate(false);
			if(do_bar){
				bar::stop();
				bar::finish(); // print out <CR> and whitespace			
			}		
			return;
		} 
		std::cout << error << "\n";
	}


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
			if(inturrupt_exit_flag){
				inturrupt_exit_flag = false;
				return;
			}
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
		case SIGFPE:
			return;


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

	std::thread progress;
	std::thread buffer;


	if(!bare){
		progress = std::thread(bar::barmanager);
		
	}else{
		bar::join();
		var::setbuffermax(0);
		var::joinbuffer();		
	}

	if(!(do_buffer && bare)){
		buffer = std::thread(var::buffer, do_buffer);
	}else{
		var::setbuffermax(0);
		var::joinbuffer();
	}
	
	// Decides whether a function is being declared or statement being evaluated
	int runtype;

	// Checks the left and right bracket count
	int lbcnt;
	int rbcnt;

	// Checks if the quotes are mismatched
	bool evenquote;
	bool sevenquote;

	std::string input;
	std::string bufferinput;
	std::string finalOutput;

	if(!bare) initbuiltin(); // Function.cpp

	mainloopstart:
	while(run){

		std::string().swap(input);
		std::string().swap(bufferinput);
		std::string().swap(finalOutput);

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

			lbcnt = checkleftBrac(input);
			rbcnt = checkrightBrac(input);
			evenquote = quotecount(input);
			sevenquote = squotecount(input);

			while(lbcnt > rbcnt || (evenquote && sevenquote)){

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
				lbcnt = checkleftBrac(input);
				rbcnt = checkrightBrac(input);
				evenquote = quotecount(input);
				sevenquote = squotecount(input);
			}
			/*
				X -1 Error - depracated pre-alpha ?
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
	// Final Cleanup
	if(progress.joinable()) progress.join();
	if(buffer.joinable()){
		var::joinbuffer();
		buffer.join();		
	}

	return 0;
}