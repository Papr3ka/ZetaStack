#include<algorithm>
#include<cctype>
#include<cstdlib>
#include<iomanip>
#include<iostream>
#include<new>
#include<sstream>
#include<string>
#include<unordered_map>
#include<vector>

#include "BuiltIn.hpp"
#include "Function.hpp"
#include "Zetacompiler.hpp"
#include "Token.hpp"

class func{
    private:
        long int argcnt;
        std::vector<token> functionargs;
        std::vector<token> functionbody;

    public:
        // Constructor
        inline func(const std::vector<token>& argvect, const std::vector<token>& functokens):
        argcnt(argvect.size()), functionargs(argvect), functionbody(functokens)
        {}

        // Deconstructor
        inline ~func(void){
        	functionargs.clear();
        	functionbody.clear();
            std::vector<token>().swap(functionargs);
            std::vector<token>().swap(functionbody);
        }


        inline std::vector<token> ret(void){
            return functionbody;
        }

        inline long int argcount(void){
            return argcnt;
        }

        inline std::vector<token> getarg(void){
            return functionargs;
        }

};

std::unordered_map< std::string , std::unordered_map<long int, func> > functiontable;

struct sfunc{
    double(*call)(std::vector<token>);
    std::string name;
    signed long int arg;
    std::vector<std::string> argsin;
};

struct corefunc{
    token(*call)(std::vector<token>);
    std::string name;
    signed long int arg;
    std::vector<std::string> argsin;
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
std::vector<token> fillvars(std::vector<token> argsname,
                            std::vector<token> argsvar,
                            std::vector<token> fbody){

    if(argsname.size() == 0) return fbody; // Return if there are no arguments
    std::vector<token> output;
    output.reserve(fbody.size());
    token varfilldata;

    while(!fbody.empty()){

        switch(fbody.front().type){
            case 5:
                varfilldata = lookup(fbody.front(), argsname, argsvar);
                output.push_back(varfilldata);
                fbody.erase(fbody.begin());
                break;
            default:
                output.push_back(fbody.front());
                fbody.erase(fbody.begin());
                break;

        }
    }
    return output;
}

std::vector<func> nfunctions; // Callable normal functions

unsigned long int filled_core = 0;
unsigned long int filled_builtin = 0;

corefunc* corefuncs = 0;
sfunc* builtinfuncs = 0;


void initcore(bool safe){

    // Macro defined in Builtin.hpp
    corefuncs = new corefunc[TOTAL_BUILTIN_CORE_FUNCS];

    if(!corefuncs){
        throw std::bad_alloc();
    }

    unsigned long int i = 0;

    if(!safe){
        corefuncs[i] = {bc_input, "input(", 0,{""}};
        i++;

        corefuncs[i] = {bc_input, "input(", 1,{"x"}};
        i++;

        corefuncs[i] = {bc_echo, "echo(", -1,{"[n]"}};
        i++;

        corefuncs[i] = {bc_abort, "abort(", 0,{""}};
        i++;

        corefuncs[i] = {bc_abort, "abort(", 1,{"EXIT_STATUS"}};
        i++;
    }

    corefuncs[i] = {bc_numcast, "ncast(", 1,{"x"}};
    i++;

    corefuncs[i] = {bc_strcast, "scast(", 1,{"x"}};
    i++;

    filled_core = i;
    return;
}


// Builtin.cpp builtin.hpp
void initbuiltin(bool safe){

    // Macro defined in Builtin.hpp
    builtinfuncs = new sfunc[TOTAL_BUILTIN_FUNCS];

    if(!builtinfuncs){
        throw std::bad_alloc();
    }

    unsigned long int i = 0; // doing it like this because it is easier to add functions

/* Template

    builtinfuncs[i] = { , "", ,{ }};
    i++;

CTRL-C + CTRL-V
*/

    // Trigonomic
    builtinfuncs[i] = {b_sin, "sin(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_cos, "cos(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_tan, "tan(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_asin, "asin(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_acos, "acos(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_atan, "atan(", 1, {"x"}};
    i++;

    // Hyperbolic
    builtinfuncs[i] = {b_sinh, "sinh(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_cosh, "cosh(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_tanh, "tanh(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_asinh, "asinh(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_acosh, "acosh(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_atanh, "atanh(", 1, {"x"}};
    i++;

    // Exponential
    builtinfuncs[i] = {b_exp, "exp(", 1, {"x"}};
    i++;

    // Logarithmic
    builtinfuncs[i] = {b_ln, "ln(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_log10, "log(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_log, "log(", 2, {"b", "x"}};
    i++;

    //
    builtinfuncs[i] = {b_sqrt, "sqrt(", 1,{"x"}};
    i++;

    builtinfuncs[i] = {b_cbrt, "cbrt(", 1,{"x"}};
    i++;

    builtinfuncs[i] = {b_root, "root(",2 ,{"n", "x"}};
    i++;
    //
    builtinfuncs[i] = {b_sum, "sum(", -1, {"[n]"}};
    i++;

    builtinfuncs[i] = {b_prod, "prod(", -1, {"[n]"}};
    i++;	

    builtinfuncs[i] = { b_avg, "avg(", -1, {"[n]"}};
    i++;

    //
    builtinfuncs[i] = {b_floor, "floor(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_ceil, "ceil(", 1, {"x"}};
    i++;	

    builtinfuncs[i] = {b_round, "round(", 1, {"x"}};
    i++;

    //
    builtinfuncs[i] = {b_min, "min(", -1, {"[n]"}};
    i++;

    builtinfuncs[i] = {b_max, "max(", -1, {"[n]"}};
    i++;

    // Disable because of Computational time
    if(!safe){
        builtinfuncs[i] = {b_zeta, "zeta(", 1, {"s"}};
        i++;
    }
    
    builtinfuncs[i] = {b_gamma, "gamma(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_factorial, "factorial(", 1, {"x"}};
    i++;

    builtinfuncs[i] = {b_erf, "erf(", 1, {"x"}};
    i++;
    
    filled_builtin = i;

    return;
}

void free_core(void){
    delete[] corefuncs;
    return;
}

void free_builtin(void){
    delete[] builtinfuncs;
    return;
}

double callspecial(std::vector<token> fargs, std::string name){
    unsigned long int index = 0;
    long int argcounts = (long int)fargs.size();
    for(unsigned long int idx = 0; idx < filled_builtin; ++idx){
        if(builtinfuncs[idx].name == name &&
          (builtinfuncs[idx].arg == -1 || builtinfuncs[idx].arg == argcounts)){
            return builtinfuncs[index].call(fargs);
        }else{
            ++index;
        }
    }
    throw 1; // Error
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
            if(corefuncs[idx].arg == -1 || corefuncs[idx].arg == argcounts){
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
    long int argcounts = (long int)fargs.size();
    for(unsigned long int idx = 0; idx < filled_core; ++idx){
        if(corefuncs[idx].name == name &&
          (corefuncs[idx].arg == -1 || corefuncs[idx].arg == argcounts)){
            return corefuncs[idx].call(fargs);
        }
    }	
	throw 1; // Error
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
   assignTo = [funcname(, L_BRAC, args ... , R_BRAC]
   body = [nums and operators ...] (must go through RPN)
*/
void def(std::vector<token> assignTo, std::vector<token> body){
    std::string name = assignTo.front().data;
    if(f_isnamespecial(name) || f_isnamecorefunc(name)){
        std::string error = "Cannot redeclare function \"";
        error.append(name).append("\"");
        throw error;
    }

    // remember that it is unsigned
    if(assignTo.size() - 3 > assignTo.size() || name == ""){
        throw std::string("Unexpected Token");
    }

    assignTo.erase(assignTo.begin());
    assignTo.erase(assignTo.begin());// Erase name and first bracket
    assignTo.pop_back(); // Erase end bracket

    if(safe_mode && !fexists(name, assignTo.size()) && funcsCount() > maxobj){
        std::cout << "Unable to declare function\n";
        return;
    }

    udef(name, assignTo.size());

    func obj(assignTo, body); // Create function object
    std::unordered_map<long int, func> &overloadtable = functiontable[name];
    overloadtable.emplace((long int)assignTo.size(), obj);
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
            return false;
        }else{
            return true;
        }
    }
}

// call function returns body list of tokens with vars filled
// format f_args = [arg1, arg2, ... ] name = name of function to be called
std::vector<token> call(std::vector<token> fargs, std::string name){
    if(!fexists(name, (long int)fargs.size())){
        std::string error = "No matching function call to \"";
        error.append(name).append("\"");
        throw error;
    }
    std::unordered_map<long int, func> &finder = functiontable[name];
    std::unordered_map<long int, func>::iterator tableIt = finder.find((long int)fargs.size());
    func &temp = tableIt->second;
    return fillvars(temp.getarg(), fargs, temp.ret());
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
                if(x+1 < tempvec.size()){
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
