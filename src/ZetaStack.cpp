/* The top level of the program
 * 
 * Copyright (c) 2020-2021 Benjamin Yao.
 * 
 * This file is part of ZetaStack.
 * 
 * ZetaStack is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * ZetaStack is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include<algorithm>
#include<array>
#include<atomic>
#include<chrono>
#include<exception>
#include<fstream>
#include<iomanip>
#include<iostream>
#include<ratio>
#include<sstream>
#include<string>
#include<thread>
#include<unordered_map>
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
#include "ZetaStack.hpp"
#include "Zetacompiler.hpp"

// Global Settings
bool run = true;            // Enable/Disables main loop
bool debug_mode = false;    // Debugging 
bool measure_time = false;  // Measuring time
bool do_exec = true;        // Execute
bool do_bar = true;         // Loading bar - extern to Builtin
bool do_cache = true;       // Cache - extern to Cache.hpp
bool do_sighandle = true;   // Handle signals
bool do_buffer = true;      // 
bool bare = false;
bool safe_mode = false;
bool evaluate_once = false;

// Default settings
/*---Variable------------Value--
 *|  run               | true  |
 *|  debug_mode        | false |
 *|  measure_time      | false |
 *|  do_exec           | true  |
 *|  do_bar            | true  |
 *|  do_cache          | true  |
 *|  do_sighandle      | true  |
 *|  do_buffer         | true  |
 *|  bare              | false |
 *|  safe_mode         | false |
 *|  evaluate_once     | false |
 *///---------------------------

/*---Unit-----Value--
 *|  RAD     | 0    |
 *|  GRAD    | 1    |
 *|  DEG     | 2    |
 *///----------------
unsigned char angle_unit = 0;

// Flags
bool inturrupt_exit_flag = false;
bool sigint_immune_flag = false;
bool sigcont_flag = false;

unsigned long int CPU_COUNT = std::thread::hardware_concurrency();

unsigned long int maxobj; // Limit to max functions and variables

long long int maxRecurse;


// Line that is printed in interface
static const constexpr char* newline = "=== "; // ≡≡≡ is what windows terminal shows, looks like a stack hence the name 𝚭Stack
static const constexpr char* multiline = "::: "; // When there is more of one bracket or quote

/* Symbols
 *  ⲗ 𝜁 ❯ 𝚭
 */

// For Displaying the unit of time when "/time" is activated
static const std::array<std::string, 4> tunit{
    " ns",
    " µs",
    " ms",
    " s"
}; // len 4

// Current Version
const constexpr version curversion = {0, 3, 6, false, -1, -1};

std::string program_name;
std::string pass_eval;

// Print out a vector element by element
inline static void printvec(std::vector<std::string> print){
    std::cout << '[';
    for(unsigned long i=0; i < print.size(); ++i){
        if(i >= 1){
            std::cout << ", ";
        }
        std::cout << print[i];
    }
    std::cout << ']';
}

inline static void printvectoken(std::vector<token> print){
    std::cout << '[';
    for(unsigned long i=0; i < print.size(); ++i){
        if(print[i].type != tok::hold &&
           print[i].type != tok::tvoid){
            if(i >= 1){
                std::cout << ", ";
            }
            std::cout << print[i].data; // .data
        }
    }
    std::cout << ']';
}

// Deal with version here [type: version -> type: string]
// Example 0.0.0-a.0
#if __cplusplus >= 202002L

inline static consteval std::string versioncomp(const version& ver){

#else

inline static std::string versioncomp(const version& ver){

#endif
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
    std::chrono::system_clock::time_point chrono_ctp;
    time_t tpointnow;
    char buffer[32];
    int time_ms;
    while(sigint_immune_flag){
        chrono_ctp = std::chrono::system_clock::now();
        tpointnow = std::chrono::system_clock::to_time_t(chrono_ctp);

        // Extra Spaces are intentional
        // Month DD YYYY HH:MM:SS:MIL
        std::strftime(buffer,sizeof(buffer),"%B %d %Y %T",std::localtime(&tpointnow));
        time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(chrono_ctp.time_since_epoch()).count() % 1000;
        if(time_ms / 10 < 1){
            std::cout << "\r" << buffer << ":00" << time_ms << "      \r";
        }else if(time_ms / 10 < 10){
            std::cout << "\r" << buffer << ":0" << time_ms << "      \r";
        }else{
            std::cout << "\r" << buffer << ":" << time_ms << "      \r";
        }
        
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }

    std::cout << "\r                                    \r";
    return;

}

// commands using the "/" symbol
inline static void command(const std::string& com){
    if(com[0] == '/'){
        return;
    }
    std::size_t shcmd_pre = com.rfind(";");
    if(shcmd_pre != std::string::npos){
        if(safe_mode){
            std::cout << "Access Denied\n";
            return;
        }
        unsigned long int shcmd_pre_pos = static_cast<unsigned long int>(shcmd_pre);
        std::string metacom = com.substr(0, shcmd_pre_pos);
        std::chrono::steady_clock::time_point tstart;
        std::chrono::steady_clock::time_point tend;
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

    if(safe_mode && cmdargv.front() != "del"){
        std::cout << "Access Denied\n";
        return;
    }

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
        if(cmdargv[1] == "force") exit(0);
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
            unsigned long long int setval;
            try{
                 setval = std::stoull(cmdargv[2]);
            }catch(const std::exception&){
                return;
            }
            
            unsigned long long int memreq = setval*sizeof(std::vector<long int>)/3145728;
            if(memreq >= 512){
                std::cout << "Warning: setting buffer to size " << setval << " requires " << memreq << " MB\n";
                std::cout << "Proceed? [Y/N] ";
                std::string confirm;
                std::getline(std::cin, confirm);

                // Detect failstate
                if(std::cin.fail()){
                    if(safe_mode){
                        exit(0);
                    }
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
            cch::clearcache();
            std::cout << "Cache cleared\n";
        }else if(cmdargv[1] == "toggle"){
            if(do_cache){
                do_cache = false;
                std::cout << "Cache disabled\n";
            }else{
                do_cache = false;
                std::cout << "Cache enabled\n";
            }
        }else if(cmdargv[1] == "setmax"){
            try{
                cch::setmaxlen(std::stoul(cmdargv[2]));
            }catch(const std::exception&){
                return;
            }
            
        }else if(cmdargv[1] == "show"){
            std::vector<ansctable> ctable = cch::getentries();
            std::cout << "Entries: " << ctable.size() << "\n";
            if(ctable.size() >= 1) std::cout << "--------\n";
            for(unsigned long int idx=0; idx < ctable.size(); ++idx){
                std::cout << "\"" << ctable[idx].key << "\": \"" << ctable[idx].ans << "\"\n";
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
    
        if(cmdargv[1] == "*"){
            var::delAll();
            return;
        }else if(cmdargv[1] == "*("){
            delAll();
            return;
        }


        int delsuccess = 0;

        // For functions, delete all no mater the argument count
        if(cmdargv[1].back() == '('){
            long int del_argcount = -1;
            try{
                del_argcount = std::stol(cmdargv[2]);
            }catch(const std::exception&){

            }

            if(del_argcount == -1){
                delsuccess = udef(cmdargv[1]);
            }else{
                delsuccess = udef(cmdargv[1], del_argcount);
            }
            
            if(delsuccess == 1){
                std::cout << "Undefined function: \"" << cmdargv[1] << "\"\n";
            }else if(delsuccess == 2){
                std::cout << "No matching overload to \"" << cmdargv[1] << "\" " << del_argcount << "\n";
            }else if(delsuccess == 3){
                std::cout << "Function \"" << cmdargv[1] << "\" Cannot be deleted\n";
            }
        }else{
            delsuccess = var::delvar(cmdargv[1]);
            if(delsuccess == 1){
                std::cout << "Undefined variable: \"" << cmdargv[1] << "\"\n";
            }else if(delsuccess == 2){
                std::cout << "Variable \"" << cmdargv[1] << "\" cannot be deleted\n";
            }
            // Sucess if delsuccess == 0
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
            --startindex;
            std::cout << "Variables: " << globalvars.size() << "\n";
            startindex = 0;
            if(globalvars.size() >= 1){
                std::sort(globalvars.begin(), globalvars.end(), [](std::string x, std::string y){return x < y;});
                std::cout << "----------\n";
                for(;startindex < globalvars.size(); ++startindex){
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
                    fast_stofloat(importbuffer.str());
                    var::update(vartoname, importbuffer.str());
                }catch(const std::invalid_argument&){
                    std::cerr << "Invalid file format\n";
                }catch(const std::string& err){
                    std::cerr << err << "\n";
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
                        if(safe_mode){
                            exit(0);
                        }
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
    }else if(cmdargv.front() == "unit"){
        std::transform(cmdargv[1].begin(), cmdargv[1].end(),cmdargv[1].begin(), ::toupper);
        if(cmdargv[1] == "DEG"){
            angle_unit = 2;
        }else if(cmdargv[1] == "GRAD"){
            angle_unit = 1;
        }else if(cmdargv[1] == "RAD"){
            angle_unit = 0;
        }else{
            std::cout << "Options for command: \"" << cmdargv.front() << "\"\n" <<
                         "    DEG\n    RAD\n    GRAD\n";
        }
        
    }else if(cmdargv.front() == "help"){
        if(cmdargv[1] == "buffer"){
            std::cout << "Options for command \"" << 
                          cmdargv[1] << "\"\n   " << 
                          cmdargv[1] << " clear\n   " << 
                          cmdargv[1] << " setmax <max>\n   " <<
                          cmdargv[1] << " size\n";
        }else if(cmdargv[1] == "cache"){
            std::cout << "Options for command \"" << 
                        cmdargv[1] << "\"\n   " << 
                        cmdargv[1] << " clear\n   " << 
                        cmdargv[1] << " toggle\n   " <<
                        cmdargv[1] << " setmax <max>\n   " <<
                        cmdargv[1] << " show\n";
        }else if(cmdargv[1] == "exit"){
            std::cout << "Options for command \"" << 
                        cmdargv[1] << "\"\n   " << 
                        cmdargv[1] << "\n   " <<
                        cmdargv[1] << " force\n";
        }else if(cmdargv[1] == "globals"){
            std::cout << "Options for command \"" << 
                        cmdargv[1] << "\"\n   " << 
                        cmdargv[1] << "\n   " <<
                        cmdargv[1] << " builtin\n";
        }else if(cmdargv[1] == "unit"){
            std::cout << "Options for command: \"" << cmdargv[1] << "\"\n" <<
                         "    DEG\n    RAD\n    GRAD\n";		
        }else{
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
                      << "   unit <unit>              Sets the unit to be used\n"
                      << "   ; <Command>              Executes system command\n";			
        }

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
    std::transform(program_name.begin(), 
                   program_name.end(),
                   program_name.begin(),
                   [](unsigned char c) -> unsigned char {return ::tolower(c);});
    // I know its over engineered
    if(!program_name.empty()){
        std::size_t lfound = program_name.find("z");
        if(lfound != std::string::npos){
            program_name[static_cast<unsigned long int>(lfound)] =
            ::toupper(program_name[static_cast<unsigned long int>(lfound)]);
        }
        lfound = program_name.find("s");
        if(lfound != std::string::npos){
            program_name[static_cast<unsigned long int>(lfound)] =
            ::toupper(program_name[static_cast<unsigned long int>(lfound)]);
        }
    }

    std::string currentarg;
    std::string peekarg;
    std::size_t splitfind;
    long long int argnum = -1;
    while(arg_index < args.size()){
        splitfind = args[arg_index].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_/!= ");
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
        if(arg_index + 1 < args.size()){
            peekarg = args[arg_index + 1];
        }else{
            peekarg = "";
        }
        if(currentarg == "--version" || currentarg == "-v"){

            if(!bare){
                std::string subline3 = "Version ";
                subline3.append(versioncomp(curversion));
                std::string subline2 = "";
                std::string subline1 = operatingsystem;

                if(detect_comp){
                    subline2.append("Built with ").append(compiler);
                    if(versioncomp(compilerversion) != ""){
                        subline2.append(" ").append(versioncomp(compilerversion));
                    }
                }

                if(subline1 == "" && subline2 == ""){
                    subline1.swap(subline3);
                    subline3 = "-";
                    subline2 = "-";

                }else if(subline2 == ""){
                    subline2.swap(subline3);
                    subline3 = "-";

                }else if(subline1 == ""){
                    subline1.swap(subline2);
                    subline2.swap(subline3);
                    subline3 = "-";
                }

                std::cout << R"( _______                    ______                  _       | -)" << '\n'
                          << R"((_______)        _         / _____) _              | |      | -)" << '\n'
                          << R"(   __    _____ _| |_ _____( (____ _| |_ _____  ____| |  _   | -)" << '\n'
                          << R"(  / /   | ___ (_   _(____ |\____ (_   _(____ |/ ___| |_/ )  | )" << subline3 <<'\n'
                          << R"( / /____| ____| | |_/ ___ |_____) )| |_/ ___ ( (___|  _ (   | )" << subline2 <<'\n'
                          << R"((_______|_____)  \__\_____(______/  \__\_____|\____|_| \_)  | )" << subline1 <<'\n';
                                                            
            }else{
                std::cout << program_name << " Version " << versioncomp(curversion) << "\n";
                if(detect_comp){
                    std::cout << "  (Built with " << compiler;
                    if(versioncomp(compilerversion) != ""){
                        std::cout << " " << versioncomp(compilerversion);
                    }

                    // Could be undef 
                    #if defined(__DATE__) && defined(__TIME__)
                        std::cout << ", " << __DATE__ << ", " << __TIME__ << ")";
                    #else
                        std::cout << ")";
                    #endif

                    if(sizeof(operatingsystem)){
                        std::cout << " on " << operatingsystem << "\n";
                    }else{
                        std::cout << "\n";
                    }
                }
            }
            exit(0);

        }else if(currentarg == "-c"){

            pass_eval = peekarg;
            evaluate_once = true;
            run = false;
            do_buffer = false;
            do_cache = false;
            if(pass_eval.front() == '"' || pass_eval.front() == '\'') pass_eval.erase(0);
            if(pass_eval.back() == '"' || pass_eval.back() == '\'') pass_eval.pop_back();
            ++arg_index;

        }else if(currentarg == "-j"){
            if(argnum != -1){
                CPU_COUNT = (unsigned long int)argnum;
            }else{
                try{
                    arg_index++;
                    CPU_COUNT = std::stoul(args[arg_index]);
                }catch(const std::exception&){
                    // error
                }	
            }
            switch(CPU_COUNT){
                case 0:
                    if(std::thread::hardware_concurrency() < 1){
                        CPU_COUNT = 1;
                    }else{
                        CPU_COUNT = std::thread::hardware_concurrency();
                    }
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
            do_cache = false;
            cch::setmaxlen(0);
            var::setbuffermax(0);
            maxRecurse = 512;

        }else if(currentarg == "--debug"){
            debug_mode = true;

        }else if(currentarg == "--nobuffer"){
            do_buffer = false;

        }else if(currentarg == "--noexec"){
            do_exec = false;

        }else if(currentarg == "--nohandle"){
            do_sighandle = false;

        }else if(currentarg == "--max-recurse"){
            if(argnum != -1){
                maxRecurse = (unsigned long long int)argnum; // used in Execute.hpp && Execute.cpp
            }else{
                try{
                    arg_index++;
                    maxRecurse = std::stoull(args[arg_index]);
                }catch(const std::exception&){
                    // error
                }
            }
        }else if(currentarg == "--safe"){
            safe_mode = true;
            do_buffer = false;
            do_buffer = false;

        }else if(currentarg == "--time"){
            measure_time = true;
        }else if(currentarg == "--help" || args[arg_index] == "-h"){
            std::cout << "Usage: " << program_name << " [Options] ...\n\n"
                      << "Options:\n"
                      << "  -h,  --help                  Display this information\n"
                      << "  -v,  --version               Display version information\n\n"
                      // Intentional Space
                      << "  -c <str>                     Evaluates a string\n"
                      << "  -j <int>                     Sets the maximum thread count\n"
                      << "  --max-recurse <int>          Sets the maximum recursion depth\n\n" // Not good idea to increase
                      // Intentional Space
                      << "  --bare                       Start with absolute minimum memory usage\n"
                      << "  --debug                      Start with debug mode on\n"
                      << "  --nobuffer                   Disables variable buffer\n"
                      << "  --noexec                     Start with execution disabled\n"
                      << "  --nohandle                   Disables signal handling\n"
                      << "  --safe                       Disables Commands and select functions\n"
                      << "  --time                       Times calculations\n";
            exit(0);
        }else{
            // Unknown argument
            std::cout << "Unknown command-line option: \""<< currentarg << "\"\n";
            exit(1);
        }
        ++arg_index;
    }
    if(run){
        std::cout << program_name << " " << versioncomp(curversion) <<"\n";
    }
    return;
}

// Work in Progress
inline static void filerun(std::string name, bool quitOnExit){
    std::ifstream execfile(name);
    if(!execfile.good()){
        execfile.close();
        std::string error = "Could not open file: \"";
        error.append(name).append("\"");
        throw error;
    }
    std::vector<std::string> filedata;
    std::string linedata;
    filedata.reserve(std::count(std::istreambuf_iterator<char>(execfile), 
             std::istreambuf_iterator<char>(), '\n'));
    linedata.reserve(64);
    while(std::getline(execfile, linedata)){
        filedata.emplace_back(linedata);
    }
    execfile.close();
    linedata.clear();
    std::string().swap(linedata);

    std::vector< std::vector<std::string> > parsedata;
    std::vector< std::vector<token> > tokendata;

    std::vector<std::string> lineparsedata;
    lineparsedata.reserve(16);

    for(std::string sline: filedata){
        parsedata.emplace_back(comp::lex(sline));
    }

    execfile.close();

    tokendata.reserve(parsedata.size());

    for(std::vector<std::string> tline: parsedata){
        tokendata.emplace_back(comp::tokenComp(tline));
    }

    parsedata.clear();
    std::vector< std::vector<std::string> >().swap(parsedata);

    comp_metadata rpn_metadata;
    std::string output;
    output.reserve(16);

    // Need to check syntax of file

    for(std::vector<token> &tline: tokendata){
        rpn_metadata = comp::getcompdata(tline);
        tline =  comp::shuntingYard(tline, rpn_metadata);
        comp::fillallvars(tline);
        output = xmath::calculate(tline, do_bar, rpn_metadata.nums);
        if(debug_mode){
            bar::setstate(false);
            std::cout << output << "\n";
        }
    }
    tokendata.clear();
    std::vector< std::vector<token> >().swap(tokendata);
    output.clear();
    std::string().swap(output);
    if(quitOnExit){
        exit(0);
    }

    return;

}

/*--Interpreting process order-----------------------------------------------Resposible Files--------------------
 *|  1. Parser "2*(14+12)" -> [2,*,(,14,+,12,)]                          |  Preprocessor.hpp / Preprocessor.cpp |
 *|  2. Lexer [2,MUL,L_BRAC,14,ADD,12,R_BRAC]                            |  Preprocessor.hpp / Preprocessor.cpp |
 *|  3. RPN [2, 14, 12, ADD, MUL]                                        |  Zetacompiler.hpp / Zetacompiler.cpp |
 *|  4. Analyzer                                                         |  Analyzer.cpp / Analyzer.hpp         |
 *|  5. Execute                                                          |  Execute.hpp / Execute.cpp           |
 *///------------------------------------------------------------------------------------------------------------

/* Note: When returning from 
 *
 *   std::string evaluate(const std::string&) 
 *  void deffunction(const std::string&)
 *
 *Always call bar::setstate(false)
 */

// Directly executing statements
inline static std::string evaluate(std::string input){
    unsigned long int tscale;
    double totaltime;
    bool usecache = true;

    if(do_bar) bar::start();
    bar::setstate(do_bar);

    inturrupt_exit_flag = true;

    // Measure time taken
    std::chrono::high_resolution_clock::time_point
        lex_tstart,
        lex_tend,
        tcomp_tstart,
        tcomp_tend,
        varfill_tstart,
        varfill_tend,
        shyd_tstart,
        shyd_tend,
        banalyze_tstart,
        banalyze_tend,
        analyze_tstart,
        analyze_tend,
        exec_tstart,
        exec_tend;
    
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
    s_out.clear();
    std::vector<std::string>().swap(s_out);

    banalyze_tstart = std::chrono::high_resolution_clock::now();
    if(!bracketOrder(output)){
        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace         
        }        
        return "Bracket mismatch";
    }
    banalyze_tend = std::chrono::high_resolution_clock::now();

    for(token dep: output){
        if(dep.type == 4 || dep.type == 5){
            usecache = false;
            break;
        }
    }

    if(cch::searchentry(input) != ""){
        bar::setstate(false);
        std::string cfinalOut = cch::searchentry(input);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace         
        }        
        if(debug_mode){
            std::cout << "Cache Hit\n\n";
        }
        try{
            var::update("ans", cfinalOut);
        }catch(const std::string& err){
            std::cerr << err << "\n";
        }
        return cfinalOut;
    }
    
    try{
        
        bar::inform("Filling Variables");
        varfill_tstart = std::chrono::high_resolution_clock::now();
        comp::fillallvars(output); // Zetacompiler.hpp
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
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace
        }  
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
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace
        }        
        return "";
    }

    if(do_exec){

        analyze_tstart = std::chrono::high_resolution_clock::now();
        if(!checkrpn(output)){
            bar::setstate(false);
            if(do_bar){
                bar::stop();
                bar::finish(); // print out <CR> and whitespace
            }			
            output.clear();
            std::vector<token>().swap(output);			
            return "Unexpected Token";
        }
        analyze_tend = std::chrono::high_resolution_clock::now();
        try{

            bar::changemode(1);
            bar::init((long int)output.size());
            bar::inform("Executing");

            exec_tstart = std::chrono::high_resolution_clock::now();
            finalOutput = xmath::calculate(output, do_bar, rpn_metadata.nums);
            exec_tend = std::chrono::high_resolution_clock::now();

        }catch(const std::string& error){
            output.clear();
            std::vector<token>().swap(output);

            xmath::resetsstreamsettings();
            bar::setstate(false);

            if(do_bar){
                bar::stop();
                bar::finish(); // print out <CR> and whitespace
            }
            return error;	
        }catch(const std::invalid_argument& err){

            output.clear();
            std::vector<token>().swap(output);			
            xmath::resetsstreamsettings();
            bar::setstate(false);

            if(do_bar){
                bar::stop();
                bar::finish(); // print out <CR> and whitespace
            }			
            return "Unexpected Token";
        }catch(const std::exception &err){

            output.clear();
            std::vector<token>().swap(output);

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
    
    output.clear();
    std::vector<token>().swap(output);

    if(finalOutput == "" && do_exec) std::cout << std::endl;

    if(measure_time){

        double lextime = std::chrono::duration<double, std::nano>(lex_tend - lex_tstart).count();
        double tcomptime = std::chrono::duration<double, std::nano>(tcomp_tend - tcomp_tstart).count();
        double varfilltime = std::chrono::duration<double, std::nano>(varfill_tend - varfill_tstart).count();
        double shydtime = std::chrono::duration<double, std::nano>(shyd_tend - shyd_tstart).count();
        double analyzetime = std::chrono::duration<double, std::nano>(analyze_tend - analyze_tstart).count() +
                             std::chrono::duration<double, std::nano>(banalyze_tend - banalyze_tstart).count();
        double exectime = std::chrono::duration<double, std::nano>(exec_tend - exec_tstart).count();
        
        totaltime = lextime+tcomptime+varfilltime+shydtime+analyzetime+exectime;

        if(totaltime < 1000){

            tscale = 0;

        }else if(totaltime >= 1000 && totaltime < 1000000){

            tscale = 1;
            lextime /= 1000;
            tcomptime /= 1000;
            varfilltime /= 1000;
            shydtime /= 1000;
            analyzetime /= 1000;
            exectime /= 1000;
            totaltime /= 1000;

        }else if(totaltime >= 1000000 && totaltime < 1000000000){

            tscale = 2;
            lextime /= 1000000;
            tcomptime /= 1000000;
            varfilltime /= 1000000;
            shydtime /= 1000000;
            analyzetime /= 1000000;
            exectime /= 1000000;
            totaltime /= 1000000;

        }else{

            tscale = 3;
            lextime /= 1000000000;
            tcomptime /= 1000000000;
            varfilltime /= 1000000000;
            shydtime /= 1000000000;
            analyzetime /= 1000000000;
            exectime /= 1000000000;
            totaltime /= 1000000000;

        }		
        std::cout << "Time variable:\n   Parse:              " << lextime << 
                 tunit[tscale] << " \n   Lexer:              " << tcomptime  <<
                 tunit[tscale] << " \n   Variable Filler:    " << varfilltime <<
                 tunit[tscale] << " \n   RPN:                " << shydtime <<
                 tunit[tscale] << " \n   Analyze:            " << analyzetime <<
                 tunit[tscale] << " \n   Execution:          " << exectime <<
                 tunit[tscale] << " \n   Total:              " << totaltime <<
                 tunit[tscale] << " \n\n";
    }
    try{
        var::update("ans",finalOutput);
    }catch(const std::string& err){
        std::cerr << err << "\n";
    }
    if(usecache) cch::addentry(input, finalOutput);
    return finalOutput;
}


// Define and Declare a function
inline static void deffunction(std::string input){
    unsigned long int tscale;
    double totaltime;

    if(do_bar) bar::start(); // Start the loading bar if option is true
    bar::setstate(do_bar);

    inturrupt_exit_flag = true;

    // Declare time points
    std::chrono::high_resolution_clock::time_point 
        lex_tstart,
        lex_tend,
        tcomp_tstart,
        tcomp_tend,
        shyd_tstart,
        shyd_tend,
        banalyze_tstart,
        banalyze_tend,
        analyze_tstart,
        analyze_tend,
        decl_tstart,
        decl_tend;

    input = comp::removeWhiteSpace(input);

    if(input.size() <= 0){
        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace         
        }
        std::cout << "Unexpected Token\n";
        return;
    }

    bar::inform("Parsing");
    lex_tstart = std::chrono::high_resolution_clock::now();
    std::vector<std::string> unsplitfunc= comp::lex(input);
    lex_tend = std::chrono::high_resolution_clock::now();

    if(debug_mode){
        bar::setstate(false);
        std::cout << "Parser:             [";
        for(std::string x: unsplitfunc){
            std::cout << x << ", ";
        }
        std::cout << "]\n";
        bar::setstate(true);
    }

    if(unsplitfunc.size() <= 0){
        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace         
        }
        std::cout << "Unexpected Token\n";
        return;
    }

    bar::inform("Lexing");
    tcomp_tstart = std::chrono::high_resolution_clock::now();
    std::vector<token> tokenfunc = comp::tokenComp(unsplitfunc);
    tcomp_tend = std::chrono::high_resolution_clock::now();

    // Free up memory
    unsplitfunc.clear();
    std::vector<std::string>().swap(unsplitfunc);

    banalyze_tstart = std::chrono::high_resolution_clock::now();
    if(!bracketOrder(tokenfunc)){
        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace         
        }
        std::cout << "Bracket mismatch\n";
        return;
    }
    dipart< std::vector<token> > splitvec = comp::spliteq(tokenfunc);
    banalyze_tend = std::chrono::high_resolution_clock::now();

    std::vector<token> variables;
    variables.reserve(splitvec.front.size());

    if(debug_mode){
        bar::setstate(false);
        std::cout << "Lexer:              [";
        for(token x: splitvec.front){

            if(x.type != tok::hold &&
               x.type != tok::tvoid)
                std::cout << x.data << ", ";

        }
        if(!splitvec.back.empty()) std::cout << tokenfunc[splitvec.front.size()].data << ", ";
        for(unsigned long int x = 0; x < splitvec.back.size(); ++x){
            
            if(splitvec.back[x].type != tok::hold &&
               splitvec.back[x].type != tok::tvoid){
                std::cout << splitvec.back[x].data;
                if(x + 1 < splitvec.back.size()){
                    std::cout << ", ";
                }
            }
        }
        std::cout << "]\n";
        bar::setstate(true);
    }

    for(unsigned long int idx = 0; idx < splitvec.front.size(); ++idx){
        if(splitvec.front[idx].type == 5 && idx + 1 < splitvec.front.size()){
            if(splitvec.front[idx + 1].type == tok::asn ||
               splitvec.front[idx + 1].type == tok::sep ||
               splitvec.front[idx + 1].type == tok::rbrac){
                variables.emplace_back(splitvec.front[idx]);
            }
        }else{
            if(splitvec.front[idx].type == 5 &&
               idx + 1 > splitvec.front.size()){
                variables.emplace_back(splitvec.front[idx]);
            }
            
        }
    }
    
    if(f_isspecial(splitvec.front.front().data, splitvec.front.size() - 3) ||
       f_iscorefunc(splitvec.front.front().data, splitvec.front.size() - 3)){
        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace			
        }

        std::cout << "Cannot redeclare function \"" << splitvec.front.front().data << "\"\n";
        return;
    }

    bar::inform("RPN");

    std::string function_name = splitvec.front.front().data;
    if(!splitvec.front.empty() && splitvec.front.front().type == tok::func) splitvec.front.erase(splitvec.front.begin());
    if(!splitvec.front.empty() && splitvec.front.front().type == tok::lbrac) splitvec.front.erase(splitvec.front.begin());
    if(!splitvec.front.empty() && 
       splitvec.front.back().type == tok::rbrac){
        splitvec.front.pop_back();
    }


    comp_metadata rpn_metadata = comp::getcompdata(splitvec.back);
    comp_metadata front_rpn_metadata = comp::getcompdata(splitvec.front);

    shyd_tstart = std::chrono::high_resolution_clock::now();
    splitvec.back = comp::shuntingYard(splitvec.back, rpn_metadata);
    splitvec.front = comp::shuntingYard(splitvec.front, front_rpn_metadata);
    shyd_tend = std::chrono::high_resolution_clock::now();

    if(debug_mode){
        bar::setstate(false);

        std::cout << "RPN:                [";
        std::cout << function_name << ", ";
        for(unsigned long int x = 0; x < splitvec.front.size(); ++x){
            if(splitvec.front[x].type != tok::hold &&
               splitvec.front[x].type != tok::tvoid) std::cout << splitvec.front[x].data << ", ";
        }
        if(!splitvec.back.empty()) std::cout << "= ,";
        for(unsigned long int x = 0; x < splitvec.back.size(); ++x){
            if(splitvec.back[x].type != tok::hold &&
               splitvec.back[x].type != tok::tvoid){
                std::cout << splitvec.back[x].data;
                if(x + 1 < splitvec.back.size()){
                    std::cout << ", ";
                }
            }
        }

        std::cout << "]\n";
        std::cout << std::endl;
        bar::setstate(true);
    }
    if(splitvec.back.size() <= 0){
        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace			
        }
        std::cout << "Unexpected Token\n";
        return;        
    }

    analyze_tstart = std::chrono::high_resolution_clock::now();
    if(!(checkrpn(splitvec.back) && checkrpn(splitvec.front))){

        bar::setstate(false);
        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace			
        }			
        std::cout << "Unexpected Token\n";
        return;
    }
    
    analyze_tend = std::chrono::high_resolution_clock::now();

    try{

        decl_tstart = std::chrono::high_resolution_clock::now();
        def(function_name, variables, splitvec.back, xmath::func_lvalue_deduction(splitvec.front));
        decl_tend = std::chrono::high_resolution_clock::now();


    }catch(const std::string& error){

        bar::setstate(false);

        splitvec.front.clear();
        splitvec.back.clear();
        variables.clear();

        std::vector<token>().swap(splitvec.front);
        std::vector<token>().swap(splitvec.back);
        std::vector<token>().swap(variables);

        xmath::resetsstreamsettings();

        if(do_bar){
            bar::stop();
            bar::finish(); // print out <CR> and whitespace			
        }

        if(error == ""){	
            return;
        } 
        std::cout << error << "\n";
        return;
    }

    splitvec.front.clear();
    splitvec.back.clear();
    variables.clear();

    std::vector<token>().swap(splitvec.front);
    std::vector<token>().swap(splitvec.back);
    std::vector<token>().swap(variables);

    xmath::resetsstreamsettings();

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
        double analyzetime = std::chrono::duration<double, std::nano>(analyze_tend - analyze_tstart).count() +
                             std::chrono::duration<double, std::nano>(banalyze_tend - banalyze_tstart).count();
        double decltime = std::chrono::duration<double, std::nano>(decl_tend - decl_tstart).count();

        totaltime = lextime+tcomptime+shydtime+analyzetime+decltime;

        if(totaltime < 1000){

            tscale = 0;

        }else if(totaltime >= 1000 && totaltime < 1000000){

            tscale = 1;
            lextime /= 1000;
            tcomptime /= 1000;
            shydtime /= 1000;
            analyzetime /= 1000;
            decltime /= 1000;
            totaltime /= 1000;

        }else if(totaltime >= 1000000 && totaltime < 1000000000){

            tscale = 2;
            lextime /= 1000000;
            tcomptime /= 1000000;
            shydtime /= 1000000;
            analyzetime /= 1000000;
            decltime /= 1000000;
            totaltime /= 1000000;

        }else{

            tscale = 3;
            lextime /= 1000000000;
            tcomptime /= 1000000000;
            shydtime /= 1000000000;
            analyzetime /= 1000000000;
            decltime /= 1000000000;
            totaltime /= 1000000000;

        }
        std::cout << "Time variable:\n   Parse:              " << lextime << 
                 tunit[tscale] << " \n   Lexer:              " << tcomptime  <<
                 tunit[tscale] << " \n   RPN:                " << shydtime <<
                 tunit[tscale] << " \n   Analyze:            " << analyzetime <<
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

                if(filled_core){
                    free_core();
                }

                if(filled_builtin){
                    free_builtin();
                }
                if(debug_mode){
                    std::cerr << "\nSignal (" << sigtype << ")\n";
                }
                exit(130);
            }

        // case SIGFPE:
        // 	return;


        //case SIGABRT:
        //	return;

        // Handles commands / child process
#ifdef SIGCHLD
        case SIGCHLD:
            return;
#endif
        // SIGCONT

        case 18:

            if(sigcont_flag){
                sigcont_flag = false;
            }
            return;

        // Handles resizing of windows
#ifdef SIGWINCH
        case SIGWINCH:
            return;
#endif

        default:
            bar::setstate(false);
            if(do_bar){
                bar::stop();
                bar::finish(); // print out <CR> and whitespace
            }
            if(filled_core){
                free_core();
            }

            if(filled_builtin){
                free_builtin();
            }
            std::cerr << "\nSignal (" << sigtype << ")\n";
            exit(128+sigtype); 
    }
}

void setenvironment(){
    if(envbit){
        maxRecurse = (envbit << 7) + (envbit << 6);
        maxobj = (envbit << 5) + (envbit << 4);
    }else{
        maxRecurse = 256;
        maxobj = 96;
    }
    return;
}

// Main entry point called by Main.cpp
int toplev_main(int argc, char** argv){

    std::vector<std::string> str_argv(argv, argv+argc);
    arghandler(str_argv);
    str_argv.clear();
    std::vector<std::string>().swap(str_argv);


    // Handle every signal
    if(do_sighandle){
        for(int signal = 1; signal <= 32; ++signal){
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

    // Initialize Function.cpp
    // Catch bad allocation
    try{
        initcore(safe_mode);
    }catch(const std::bad_alloc&){
        std::cerr << "Initialization of core functions failed\n";
    }

    if(!bare){
        try{
            initbuiltin(safe_mode);
        }catch(const std::bad_alloc&){
            std::cerr << "Initialization of built-in functions failed\n";
        } 
    }

    if(evaluate_once){
        pass_eval = comp::removeWhiteSpace(comp::stripcomment(pass_eval));

        lbcnt = checkleftBrac(pass_eval);
        rbcnt = checkrightBrac(pass_eval);
        evenquote = quotecount(pass_eval);
        sevenquote = squotecount(pass_eval);

        if(lbcnt != rbcnt){
            std::cout << "Bracket mismatch\n";
            goto mainloopfinish;
        }

        if(evenquote && sevenquote){
            std::cout << "Unmatching quotes\n";
            goto mainloopfinish; 
        }

        std::string final_ans = evaluate(pass_eval);

        pass_eval.clear();
        std::string().swap(pass_eval);

        if(final_ans != ""){
            std::cout << final_ans << std::endl;

            final_ans.clear();
            std::string().swap(final_ans);
        }
         
    }

    
    mainloopstart:

    while(run){

        inturrupt_exit_flag = false;

        std::cout << newline;
        std::getline(std::cin, input);

        // Check if CTRL+Z or CIN is in fail state and clear it
        if(std::cin.fail()){
            if(safe_mode){
                goto mainloopfinish;
            }
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
                    if(safe_mode){
                        goto mainloopfinish;
                    }
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
                std::cout << finalOutput << std::endl;

                // Cleanup
                if(!bare && do_buffer) var::clearbuffer();

                input.clear();
                bufferinput.clear();
                finalOutput.clear();

                std::string().swap(input);
                std::string().swap(bufferinput);
                std::string().swap(finalOutput);
                
            }
        }else{
            // commands
            input.erase(input.begin());
            command(input);
        }
    }

    // Jump from nested loop
    mainloopfinish:

    // Final Cleanup
    bar::join();
    bar::stop();

    if(filled_core){
        free_core();
    }

    if(filled_builtin){
        free_builtin();
    }

    if(progress.joinable()) progress.join();

    if(buffer.joinable()){
        var::joinbuffer();
        buffer.join();		
    }

    return 0;
}