/* Interface for calling, defining and modifying functions
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
#include<cctype>
#include<cstdlib>
#include<iomanip>
#include<iostream>
#include<new>
#include<sstream>
#include<string>
#include<unordered_map>
#include<unordered_set>
#include<vector>

#include "BuiltIn.hpp"
#include "Execute.hpp"
#include "Function.hpp"
#include "ZetaStack.hpp"
#include "Zetacompiler.hpp"

class func{
    private:
        long int argcnt;
        std::vector<token> functionargs;
        std::vector<token> functionbody;
    public:

        std::unordered_map<std::string, default_modifier> defaults;

        // Constructor
        inline func(const std::vector<token>& argvectm,
                    const std::vector<token>& functokensm,
                    const std::unordered_map<std::string, default_modifier>& defualtsm):
        argcnt(argvectm.size()), functionargs(argvectm), functionbody(functokensm), defaults(defualtsm)
        {}

        inline func(){}

        // Deconstructor
        inline ~func(void){
        	functionargs.clear();
        	functionbody.clear();
            std::vector<token>().swap(functionargs);
            std::vector<token>().swap(functionbody);
            defaults.clear();
        }


        inline std::vector<token> ret(void){
            return functionbody;
        }

        inline long int argcount(void){
            return argcnt;
        }

        inline std::vector<token> getarg(void){
            return this->functionargs;
        }

        inline unsigned long int defsize(void){
            unsigned long int count = 0;
            for(std::pair<const std::string, default_modifier> x: defaults){
                if(x.second.asn_type.data == "ASN") ++count;
            }
            return count;
        }

};

std::unordered_map< std::string , std::unordered_map<long int, func> > functiontable;

template<class T>
struct funcscore{
    T(*call)(std::vector<token>);
    std::string name;
    signed long int arg;
    std::vector<std::string> argsin;
    unsigned long int minarg;
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
std::vector<token> fillvars(const std::vector<token>& argsname,
                            const std::vector<token>& argsvar,
                            const std::vector<token>& fbody){

    if(argsname.size() == 0) return fbody; // Return if there are no arguments
    std::vector<token> output;
    output.reserve(fbody.size());
    token varfilldata;

    for(const token& tk:fbody){

        switch(tk.type){
            case tok::var:
                varfilldata = lookup(tk, argsname, argsvar);
                output.emplace_back(varfilldata);
                break;
            default:
                output.emplace_back(tk);
                break;

        }
    }
    return output;
}

std::vector<func> nfunctions; // Callable normal functions

unsigned long int filled_core = 0;
unsigned long int filled_builtin = 0;

funcscore<token>* corefuncs = 0;
funcscore<double>* builtinfuncs = 0;


void initcore(bool safe){

    // Macro defined in Builtin.hpp
    corefuncs = new funcscore<token>[TOTAL_BUILTIN_CORE_FUNCS];

    if(!corefuncs){
        throw std::bad_alloc();
    }

    unsigned long int i = 0;

    if(!safe){
        corefuncs[i] = {bc_input, "input(", 0,{""}, 0};
        i++;

        corefuncs[i] = {bc_input, "input(", 1,{"x"}, 1};
        i++;

        corefuncs[i] = {bc_echo, "echo(", -1,{"[n]"}, 1};
        i++;

        corefuncs[i] = {bc_abort, "abort(", 0,{""}, 0};
        i++;

        corefuncs[i] = {bc_abort, "abort(", 1,{"EXIT_STATUS"}, 1};
        i++;
    }

    corefuncs[i] = {bc_numcast, "ncast(", 1,{"x"}, 1};
    i++;

    corefuncs[i] = {bc_strcast, "scast(", 1,{"x"}, 1};
    i++;

    if(!safe){
        // bc_sleep uses spin lock while bc_safe_sleep uses a timed spin lock

        corefuncs[i] = {bc_sleep, "sleep(", 1,{"x"}, 1};
    }else{
        corefuncs[i] = {bc_safe_sleep, "sleep(", 1,{"x"}, 1};
    }
    i++;

    filled_core = i;
    return;
}


// Builtin.cpp builtin.hpp
void initbuiltin(bool safe){

    // Macro defined in Builtin.hpp
    builtinfuncs = new funcscore<double>[TOTAL_BUILTIN_FUNCS];

    if(!builtinfuncs){
        throw std::bad_alloc();
    }

    unsigned long int i = 0; // doing it like this because it is easier to add functions

/* Template
    builtinfuncs[i] = { , "", ,{ }, };
    i++;
CTRL-C + CTRL-V
*/

    // Trigonomic
    builtinfuncs[i] = {b_sin, "sin(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_cos, "cos(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_tan, "tan(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_asin, "asin(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_acos, "acos(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_atan, "atan(", 1, {"x"}, 1};
    i++;

    // Hyperbolic
    builtinfuncs[i] = {b_sinh, "sinh(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_cosh, "cosh(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_tanh, "tanh(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_asinh, "asinh(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_acosh, "acosh(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_atanh, "atanh(", 1, {"x"}, 1};
    i++;

    // Exponential
    builtinfuncs[i] = {b_exp, "exp(", 1, {"x"}, 1};
    i++;

    // Logarithmic
    builtinfuncs[i] = {b_ln, "ln(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_log10, "log(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_log, "log(", 2, {"b", "x"}, 2};
    i++;

    //
    builtinfuncs[i] = {b_sqrt, "sqrt(", 1,{"x"}, 1};
    i++;

    builtinfuncs[i] = {b_cbrt, "cbrt(", 1,{"x"}, 1};
    i++;

    builtinfuncs[i] = {b_root, "root(",2 ,{"n", "x"}, 2};
    i++;
    //
    builtinfuncs[i] = {b_sum, "sum(", -1, {"[n]"}, 1};
    i++;

    builtinfuncs[i] = {b_prod, "prod(", -1, {"[n]"}, 1};
    i++;	

    builtinfuncs[i] = {b_avg, "avg(", -1, {"[n]"}, 1};
    i++;

    //
    builtinfuncs[i] = {b_floor, "floor(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_ceil, "ceil(", 1, {"x"}, 1};
    i++;	

    builtinfuncs[i] = {b_round, "round(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_abs, "abs(", 1, {"x"}, 1};
    i++;

    //
    builtinfuncs[i] = {b_min, "min(", -1, {"[n]"}, 1};
    i++;

    builtinfuncs[i] = {b_max, "max(", -1, {"[n]"}, 1};
    i++;

    // Disable because of Computational time
    if(!safe){
        builtinfuncs[i] = {b_zeta, "zeta(", 1, {"s"}, 1};
        i++;
    }
    
    builtinfuncs[i] = {b_gamma, "gamma(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_factorial, "factorial(", 1, {"x"}, 1};
    i++;

    builtinfuncs[i] = {b_erf, "erf(", 1, {"x"}, 1};
    i++;
    
    filled_builtin = i;

    return;
}

void free_core(void){
    delete[] corefuncs;
    corefuncs = 0;
    return;
}

void free_builtin(void){
    delete[] builtinfuncs;
    builtinfuncs = 0;
    return;
}

bool returns(std::string fname, long int argcount){
    if(fname == "echo(" && argcount == 1) return false;
    if(fname == "sleep(" && argcount == 1) return false;
    if(fname == "abort(" && argcount == 1) return false;
    return true;
}

double callspecial(std::vector<token> fargs, std::string name){
    unsigned long int index = 0;
    for(unsigned long int idx = 0; idx < filled_builtin; ++idx){
        if(builtinfuncs[idx].name == name &&
          (builtinfuncs[idx].arg == -1 || builtinfuncs[idx].arg == (long int)fargs.size()) &&
          builtinfuncs[idx].minarg <= fargs.size()){
            return builtinfuncs[index].call(fargs);
        }else{
            ++index;
        }
    }

    std::string error = "No matching function call to \"";
    error.append(name).append("\"");
    throw error;
}

bool f_isspecial(std::string name, long int argcounts){
    for(unsigned long int idx = 0; idx < filled_builtin; ++idx){
        if(builtinfuncs[idx].name == name){
            if(builtinfuncs[idx].arg == -1 || builtinfuncs[idx].arg == argcounts){
                return true;
            }
        }
    }
    return false;
}

bool f_isnamespecial(std::string name){
    for(unsigned long int idx = 0; idx < filled_builtin; ++idx){
        if(builtinfuncs[idx].name == name){
            return true;
        }
    }
    return false;
}

bool f_iscorefunc(std::string name, long int argcounts){
    for(unsigned long int idx = 0; idx < filled_core; ++idx){
        if(corefuncs[idx].name == name){
            if((corefuncs[idx].arg == -1 ||
               corefuncs[idx].arg == argcounts) && 
               argcounts >= (long int)corefuncs[idx].minarg){
                return true;
            }
        }
    }
    return false;
}


bool f_isnamecorefunc(std::string name){
    for(unsigned long int idx = 0; idx < filled_core; ++idx){
        if(corefuncs[idx].name == name){
            return true;
        }
    }
    return false;
}

token callcore(std::vector<token> fargs, std::string name){
    for(unsigned long int idx = 0; idx < filled_core; ++idx){
        if(corefuncs[idx].name == name &&
          (corefuncs[idx].arg == -1 || corefuncs[idx].arg == (long int)fargs.size())){
            return corefuncs[idx].call(fargs);
        }
    }	
    std::string error = "No matching function call to \"";
    error.append(name).append("\"");
    throw error;
}


void delAll(void){
    functiontable.clear();
    std::unordered_map< std::string , std::unordered_map<long int, func> >().swap(functiontable);
    return;
}


// Args name = name of function, argcounts = amount of arguments -1 if unspecified 
// 0 = Success, 1 = function not found, 2 = function found but no matching argument, 3 = Unable to delete
// delete function
int udef(std::string name, long int argcounts){
    if(f_isnamespecial(name) || f_isnamecorefunc(name)) return 3;
    if(functiontable.find(name) == functiontable.end()){
        return 1;
    }
    if(argcounts != -1){
        std::unordered_map< std::string , std::unordered_map<long int, func> >::iterator iter = functiontable.find(name);
        if(iter != functiontable.end()){
            std::unordered_map<long int, func>::iterator argIterator = iter->second.find(argcounts);
            if(argIterator != iter->second.end()){
                iter->second.erase(argIterator);
                if(iter->second.size() <= 0){
                    functiontable.erase(iter);
                }
                return 0;
            }
        }
        return 2;
        
        
    }else{
        std::unordered_map< std::string , std::unordered_map<long int, func> >::iterator iter = functiontable.find(name);
        if(iter != functiontable.end()){
            functiontable.erase(iter);
        }
        return 0;
    }
}

/* Define function
   Vector format 
   assignTo = [args ... ]
   body = [nums and operators ...] (must go through RPN)
*/
void def(std::string name, std::vector<token> assignTo, std::vector<token> body, std::unordered_map<std::string, default_modifier> defaults){
    if(f_isnamespecial(name) || f_isnamecorefunc(name)){
        std::string error = "Cannot redeclare function \"";
        error.append(name).append("\"");
        throw error;
    }

    if(name.back() != '('){
        throw std::string("Invalid function name");        
    }

    if(safe_mode && !fexists(name, assignTo.size()) && funcsCount() > maxobj){
        std::cout << "Unable to declare function\n";
        return;
    }

    udef(name, assignTo.size());

    func obj(assignTo, body, defaults); // Create function object
    std::unordered_map<long int, func> &overloadtable = functiontable[name];

    overloadtable.rehash(overloadtable.size()+1);

    #if __cplusplus >= 201703L
        overloadtable.try_emplace((long int)assignTo.size(), obj);
    #else
        overloadtable.emplace((long int)assignTo.size(), obj);
    #endif

    return;
}

// Returns true if function with n-args exists else false
bool fexists(std::string name, signed long int argcounts){
    if(functiontable.find(name) == functiontable.end()){
        return false;
    }else{
        if(argcounts == -1) return true;
        std::unordered_map<long int, func> &finder = functiontable[name];
        if(finder.find(argcounts) == finder.end()){
            std::unordered_map< std::string, std::unordered_map<long, func> >::iterator overload_table = functiontable.find(name);
            for(std::unordered_map<long, func>::iterator it = overload_table->second.begin();
                it != overload_table->second.end();
                it++){

                if(argcounts >= (it->first - (signed long int)it->second.defsize()) &&
                   argcounts <= it->first) return true;
            }
            return false;
        }else{
            return true;
        }
    }
}

template<class T>
inline bool invec(const std::vector<T>& vec, T item){
    for(const T search: vec){
        if(search == item) return true;
    }
    return false;
}


// call function returns body list of tokens with vars filled
// format f_args = [arg1, arg2, ... ] name = name of function to be called
std::vector<token> call(std::vector<token> fargs, std::string name){
    if(!fexists(name, (long int)fargs.size())){
        std::string error = "No matching function call to \"";
        error.append(name).append("\"");
        throw error;
    }

    std::unordered_map< std::string , std::unordered_map<long int, func> >::iterator overload_table = functiontable.find(name);
    std::unordered_map<long int, func>::iterator overload_resolution = overload_table->second.end();

    std::vector<token> callargs;

    for(std::unordered_map<long, func>::iterator it = overload_table->second.begin();
        it != overload_table->second.end();
        it++){

        if(fargs.size() >= ((unsigned long int)it->first - it->second.defsize()) &&
           fargs.size() <= (unsigned long int)it->first){

                overload_resolution = it;
                callargs = overload_resolution->second.getarg();
                break;
        }
    }

    // Safety check for end iterator
    if(overload_resolution == overload_table->second.end()){
        std::string error = "No matching function call to \"";
        error.append(name).append("\"");
        throw error;
    }

    fargs.reserve(callargs.size());

    // Fill modified arguments
    for(unsigned long int idx = 0; idx < fargs.size(); ++idx){
        if(overload_resolution->second.defaults.find(callargs[idx].data) !=
           overload_resolution->second.defaults.end() &&
           overload_resolution->second.defaults[callargs[idx].data].initialized){

            // ASN is reserved for filling missing args
            if(overload_resolution->second.defaults[callargs[idx].data].asn_type.data != "ASN" &&
               fargs[idx].type != tok::hold){

               fargs[idx] = xmath::apply(overload_resolution->second.defaults[callargs[idx].data], fargs[idx]);

            }else if(fargs[idx].type == tok::hold){

                fargs[idx] = xmath::apply(overload_resolution->second.defaults[callargs[idx].data], token());
                if(fargs[idx].type == tok::tvoid){
                    std::string error = "No matching function call to \"";
                    error.append(name).append("\"");
                    throw error;                  
                }
            }

        }
    }

    // Fill missing arguments
    while(callargs.size() > fargs.size()){
        if(overload_resolution->second.defaults.find(callargs[fargs.size()].data) !=
           overload_resolution->second.defaults.end()){
               fargs.emplace_back(xmath::apply(overload_resolution->second.defaults[callargs[fargs.size()].data], token()));    
         }else{
            std::string error = "No matching function call to \"";
            error.append(name).append("\"");
            throw error;
        }
    }

    // Fill variables in function body then return it
    return fillvars(callargs, fargs, overload_resolution->second.ret());
}

static inline std::string to_string_hprecd(double x){
    std::ostringstream doublestring;
    doublestring << std::setprecision(std::numeric_limits<double>::digits10) << x;
    return doublestring.str();
}


// Returns argument count of first occurence of function name
long int argcount(std::string name){
    std::unordered_map<long int, func> &temp = functiontable[name];
    for(std::unordered_map<long int, func>::iterator agcidx = temp.begin();
        agcidx != temp.end(); agcidx++){
        return agcidx->first;
    }
    return -1;
}

unsigned long int funcsCount(void){
    unsigned long int count = 0;
    for(std::unordered_map< std::string ,std::unordered_map<long int, func> >::iterator
        itx = functiontable.begin();
        itx != functiontable.end();
        itx++){
            count++;
            count += itx->second.size();
        }
    return count;
}

std::vector<std::string> getandassemble_all_defined_functions(void){
    std::vector<std::string> output;
    std::vector<token> tempvec;
    output.reserve(1+functiontable.size());
    std::string tempstring;
    tempstring.reserve(16);
    std::vector<token> tempargs;
    for(std::unordered_map< std::string ,std::unordered_map<long int, func> >::iterator
        itx = functiontable.begin();
        itx != functiontable.end();
        itx++){

        for(std::unordered_map<long int, func>::iterator
            itf = itx->second.begin();
            itf != itx->second.end();
            itf++){

            tempstring = itx->first;
            tempvec = itf->second.getarg();

            for(unsigned long int x=0; x < tempvec.size(); x++){
                
                tempstring.append(tempvec[x].data);

                // Account for rvalue deduction
                
                if(!itf->second.defaults[tempvec[x].data].asn_type.data.empty()){

                    tempstring.append(" ").append(comp::unmangle(itf->second.defaults[tempvec[x].data].asn_type));
                }

                if(!itf->second.defaults[tempvec[x].data].rvalue.data.empty()){

                    tempstring.append(" ").append(itf->second.defaults[tempvec[x].data].rvalue.data);
                }

                if(x + 1 < tempvec.size()){

                    tempstring.append(", ");
                }
                
            }
            tempstring.append(")");
            output.emplace_back(tempstring);
        }
    }
    
    return output;
}

std::vector<std::string> getandassemble_all_builtin_functions(void){
    std::vector<std::string> output;
    output.reserve(nfunctions.size());
    std::string tempstring;
    tempstring.reserve(TOTAL_BUILTIN_FUNCS);
    unsigned long int index = 0;
    for(unsigned long int idx = 0; idx < filled_builtin; ++idx){
        tempstring = builtinfuncs[idx].name;
        if(tempstring != ""){
            for(index = 0; index < builtinfuncs[idx].argsin.size(); index++){
                tempstring.append(builtinfuncs[idx].argsin[index]);
                if(index + 1 < builtinfuncs[idx].argsin.size()){
                    tempstring.append(", ");
                }
            }
            tempstring.append(")");
            output.emplace_back(tempstring);
        }
    }
    return output;
}