/* Header file for files that link with main.cpp.
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

#ifndef ZETASTACK_HPP
#define ZETASTACK_HPP

#include<array>
#include<thread>
#include<string>
#include<unordered_map>

// Global Settings
extern bool run;           
extern bool debug_mode;   
extern bool measure_time;
extern bool do_exec;      
extern bool do_bar;    
extern bool do_cache;
extern bool do_sighandle;
extern bool do_buffer;      
extern bool bare;
extern bool safe_mode;
extern bool evaluate_once;
extern bool rng_type;

// Flags
extern bool inturrupt_exit_flag;
extern bool sigint_immune_flag;
extern bool sigcont_flag;

// For versioning
struct version{
    int major;
    int minor;
    int rev;

    bool special;
    int vertype;
    int specialrev;

    //version() = default;
};

extern long long int maxRecurse;
extern unsigned long int maxobj;

extern unsigned long int CPU_COUNT;
extern unsigned char angle_unit;

///////////////////////////////

// Private Build removes all information that can identify the build
#ifndef PRIVATE_BUILD

    // Version detect for compilers
    #if defined(__clang__)
        static const bool detect_comp = true;
        static const constexpr version compilerversion = {__clang_major__,
                                                          __clang_minor__,
                                                          __clang_patchlevel__,
                                                          false,
                                                          -1,
                                                          -1};
        static const constexpr char* compiler = "Clang";
    #elif defined(__IBMCPP__)
        static const bool detect_comp = true;
        static const constexpr char* compiler = "XLC";
        static const constexpr version compilerversion = {-1,-1,-1, false, -1, -1};        
    #elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC)
        static const bool detect_comp = true;
        static const constexpr char* compiler = "ICC";
        static const constexpr version compilerversion = {-1,-1,-1, false, -1, -1};
    #elif defined(__GNUC__) && defined(__GNUG__)
        static const bool detect_comp = true;
        static const constexpr version compilerversion = {__GNUC__,
                                                          __GNUC_MINOR__,
                                                          __GNUC_PATCHLEVEL__,
                                                          false,
                                                          -1,
                                                          -1};
        static const constexpr char* compiler = "GCC";
    #elif defined(_MSC_VER)
        static const bool detect_comp = true;
        static const constexpr char* compiler = "MSVC";
        static const constexpr version compilerversion = {-1,-1,-1, false, -1, -1};
    #elif defined(__NVCC__)
        static const bool detect_comp = true;
        static const constexpr char* compiler = "NVCC";
        static const constexpr version compilerversion = {__CUDACC_VER_MAJOR__,
                                                          __CUDACC_VER_MINOR__,
                                                          __CUDACC_VER_BUILD__,
                                                          false,
                                                          -1,
                                                          -1};
    #elif defined(__PGIC__)
        static const bool detect_comp = true;
        static const constexpr char* compiler = "PGC";
        static const constexpr version compilerversion = {__PGIC__,
                                                          __PGIC_MINOR__ ,
                                                          __PGIC_PATCHLEVEL__,
                                                          false,
                                                          -1,
                                                          -1};
    #else
        static const bool detect_comp = false;
        static const constexpr version compilerversion = {-1,-1,-1, false, -1, -1};
        static const constexpr char* compiler = "Unknown";
    #endif
    // Detect OS
    #if defined(_WIN16) || defined(_WIN32) || \
        defined(__WIN32__) || defined(_WIN64) || \
        defined(_WIN32_WCE) || defined(__TOS_WIN__) || \
        defined(__WINDOWS__)
        static const constexpr char* operatingsystem = "Windows";
    #elif defined(_AIX) || defined(__TOS_AIX__)
        static const constexpr char* operatingsystem = "AIX";
    #elif defined(__APPLE__) || defined(__MACH__)
        static const constexpr char* operatingsystem = "Mac OSX";
    #elif defined(__linux__)
        static const constexpr char* operatingsystem = "Linux";
    #elif defined(__FreeBSD__) 
        static const constexpr char* operatingsystem = "FreeBSD";
    #elif defined(__NetBSD__)
        static const constexpr char* operatingsystem = "NetBSD";
    #elif defined(__OpenBSD__)
        static const constexpr char* operatingsystem = "OpenBSD";
    #elif defined(__DragonFly__)
        static const constexpr char* operatingsystem = "DragonFly";
    #elif defined(__CYGWIN__)
        static const constexpr char* operatingsystem = "Cygwin";
    #elif defined(sun) || defined(__sun)
        static const constexpr char* operatingsystem = "Solaris";    
    #elif defined(unix) || defined(__unix) || defined(__unix__)
        static const constexpr char* operatingsystem = "Unix";
    #else
        static const constexpr char* operatingsystem = "";
    #endif
#else
    // Private build removes all information about when and what built the program

    #undef __DATE__
    #undef __TIME__

    static const bool detect_comp = false;
    static const constexpr char* compiler = "";
    static const constexpr version compilerversion = {-1,-1,-1, false, -1, -1};
    static const constexpr char* operatingsystem = "";

#endif

void setenvironment(int envbit);

// Top Level Main Entry point

///////////////////////////////

int toplev_main(int argc, char** argv);

///////////////////////////////

namespace tok{

    // Operator
    enum tokentype{
        // void
        tvoid = -(1 << 8) + 1,
        // Placeholder
        hold = -1,
        // Number
        num = 0,
        // Operator
        op = 1,
        // Left bracket
        lbrac = 2,
        // Right bracket
        rbrac = 3,
        // Function
        func = 4,
        // Variable
        var = 5,
        // Right function
        rfunc = 6,
        // Separator
        sep = 7,
        // Keyword
        key = 8,
        // String
        str = 9,
        // Assigning operator
        asn = 10
    };

}


class token{
    public:

        std::string data;
        long int type;
        long int reserved;
        long int special;

        // overloaded constructors for optional parameters
        inline token():
        data(""), type(tok::tvoid), reserved(tok::tvoid), special(tok::tvoid)
        {}
        
        inline token(const std::string& data_,
                     long int type_):
        data(data_), type(type_), reserved(tok::tvoid), special(tok::tvoid)
        {}

        inline token(const std::string& data_,
                     long int type_,
                     const long int reserved_):
        data(data_), type(type_), reserved(reserved_), special(tok::tvoid)
        {}

        inline token(const std::string& data_,
                     long int type_,
                     const long int reserved_,
                     long int special_):
        data(data_), type(type_), reserved(reserved_), special(special_)
        {}

        inline ~token(){
            data.clear();
            std::string().swap(data);
        }

        // comparisons
        inline bool operator != (token x){
            return !(this->data == x.data &&
                    this->type == x.type &&
                    this->reserved == x.reserved &&
                    this->special == x.special);
        }

        inline bool operator == (token x){
            return (this->data == x.data &&
                    this->type == x.type &&
                    this->reserved == x.reserved &&
                    this->special == x.special);
        }

        inline friend bool operator == (std::string a, token b){
            if(a == b.data) return true;
            return false;
        }

        // Required for <string> to compare token with const char []
        inline bool operator == (const char mval[]){
            if(this->data.c_str() == mval) return true;
            return false;
        }

};

struct tokenHash{
    std::size_t operator()(const token& k) const {
        std::size_t res = 17;
        res = res * 31 + std::hash<std::string>()(k.data);
        res = res * 31 + std::hash<long int>()(k.type);
        res = res * 31 + std::hash<long int>()(k.reserved);
        res = res * 31 + std::hash<long int>()(k.special);
        return res;
    }
};

#endif