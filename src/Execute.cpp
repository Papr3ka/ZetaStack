
#include<algorithm>
#include<array>
#include<cmath>
#include<cstdlib>
#include<iomanip>
#include<iostream>
#include<limits>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>

#include "Cache.hpp"
#include "Execute.hpp"
#include "Function.hpp"
#include "Status.hpp"
#include "Token.hpp"
#include "Variable.hpp"
#include "Zetacompiler.hpp"

namespace xmath {

    long long int maxRecurse = 1280; // Dont touch this unless your platform has bigger stack size
    long long int recursionCount = 0;

    void setmaxrecurse(long long maxR){
        maxRecurse = maxR;
        return;
    }

    long long getmaxrecurse(void){
        return maxRecurse;
    }

    inline std::string to_string_hprec(double x){
        std::ostringstream doublestring;
        doublestring << std::setprecision(std::numeric_limits<double>::digits10) << x;
        return doublestring.str();
    }

    void resetsstreamsettings(void){
        std::stringstream strstream(std::stringstream::in | std::stringstream::out);
        strstream << std::setprecision(6);
        return;
    }

    inline std::string to_string_hprec(long long x){
        std::ostringstream llstring;
        llstring << std::setprecision(std::numeric_limits<long long>::digits10) << x;
        return llstring.str();
    }

    inline std::string to_string_hprec(bool x){
        if(x){
            return "1";
        }else{
            return "0";
        }
    }

    inline bool string_isnum(std::string str){
        if(str.size() <= 1){
            if(isdigit(str[0]) || str[0] == '.'){
                return true;
            }else{
                return false;
            }
        }else{ // eg -. would be considerd as -0
            if(isdigit(str[0]) || 
               isdigit(str[1]) || 
                 str[0] == '.' || 
                 str[1] == '.'){
                return true;
            }else{
                return false;
            }
        }
    }

    inline std::string addstr(std::string x, std::string y){
    x.append(y);
        return x;
    }

    // Placeholder functions for Arbitrary math
    inline void add(std::vector<token>& evalstack){
        if(evalstack.back().type == 0){
            double y = std::stod(evalstack.back().data);
            evalstack.pop_back();
            double x = std::stod(evalstack.back().data);
            evalstack.back() = token(to_string_hprec(x+y), 0);
        }else{
            std::string y = evalstack.back().data;
            evalstack.pop_back();
            std::string x = evalstack.back().data;
            evalstack.back() = token(x.append(y), 9);
        }
        return;
    }

    inline void sub(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x - y), 0);
        return;
    }

    inline void mul(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x * y), 0);
        return;
    }

    inline void divide(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x / y), 0);
        return;
    }

    inline void mod(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(fmod(x, y)), 0);
        return;
    }

    inline void power(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(pow(x, y)), 0);
        return;
    }

    inline void bitexclusiveOr(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x ^ y), 0);
        return;
    }

    inline void shl(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x << y), 0);
        return;
    }

    inline void shr(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x >> y), 0);
        return;
    }

    inline void eql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x == y), 0);
        return;
    }

    inline void nql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x != y), 0);
        return;
    }

    inline void gql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x >= y), 0);
        return;
    }

    inline void lql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x <= y), 0);
        return;
    }

    inline void grt(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x > y), 0);
        return;
    }

    inline void lst(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x < y), 0);
        return;
    }

    inline void neg(std::vector<token>& evalstack){
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(-x), 0);
        return;
    }

    inline void bitwor(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x | y), 0);
        return;
    }

    inline void bitwand(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x & y), 0);
        return;
    }

    inline void land(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x && y), 0);
        return;
    }

    inline void lor(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x || y), 0);
        return;
    }

    inline void lnot(std::vector<token>& evalstack){
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(!x), 0);
        return;
    }

    inline void asn(std::vector<token>& evalstack){
        token val = evalstack.back();
        evalstack.pop_back();

        if(val.type == 9){
            std::string iden = evalstack.back().data;
            evalstack.pop_back();
            if(var::changable(iden)){
                var::update(iden, val.data);
                return;
            }else{
                std::string error = "Variable \"";
                error.append(iden).append("\" is read-only");
                throw error;
            }   
        }else{
            std::string iden = evalstack.back().data;
            evalstack.pop_back();
            if(var::changable(iden)){
                var::update(iden, to_string_hprec(std::stod(val.data)));
                return;
            }else{
                std::string error = "Variable \"";
                error.append(iden).append("\" is read-only");
                throw error;
            }
        }

    }

    inline void addasn(std::vector<token>& evalstack){
        token val = evalstack.back();
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(!var::changable(iden)){
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
        if(val.type == 9){
            var::update(iden,
                var::search(iden).append(val.data)
            );
        }else{
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) + std::stod(val.data)
            ));
            return;  
        }
        
    }

    inline void subasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) - std::stod(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void mulasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) * std::stod(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void divasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) / std::stod(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void modasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                fmod(std::stod(var::search(iden)), std::stod(val))
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void exclusiveOrasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stoll(var::search(iden)) ^ std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void powasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                pow(std::stod(var::search(iden)), std::stod(val))
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void shlasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) << std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void shrasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) >> std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitexclusiveOrasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();        
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) ^ std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }


    inline void bitorasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();        
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) | std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitandasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();        
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) & std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    std::unordered_map<std::string, void(*)(std::vector<token>&)> ExecOperators = {
        {"ADD", add},
        {"SUB", sub},
        {"MUL", mul},
        {"DIV", divide},
        {"MOD", mod},
        {"POW", power},
        {"XOR", bitexclusiveOr},
        {"SHL", shl},
        {"SHR", shr},
        {"EQL", eql},
        {"NQL", nql},
        {"GQL", gql},
        {"LQL", lql},
        {"GRT", grt},
        {"LST", lst},
        {"NEG", neg},
        {"POS", [](std::vector<token>&)->void{return;}},
        {"BITOR", bitwor},
        {"BITAND", bitwand},
        {"BITXOR", bitexclusiveOr},
        {"LAND", land},
        {"LOR", lor},
        {"NOT", lnot},
        {"ASN", asn},
        {"SUBASN", subasn},
        {"ADDASN", addasn},
        {"MULASN", mulasn},
        {"DIVASN", divasn},
        {"MODASN", modasn},
        {"POWASN", powasn},
        {"SHLASN", shlasn},
        {"SHRASN", shrasn},
        {"BITXORASN", exclusiveOrasn},
        {"BITORASN", bitorasn},
        {"BITANDASN", bitandasn}

    };



    std::vector<token> simplify(std::vector<token> tokens, std::vector<token> targets, std::string freserved, unsigned long int reservecount){
        if(recursionCount > maxRecurse){
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        ++recursionCount;

        bool dumpfunction = false;

        std::vector<token> evalstack;
        evalstack.reserve(reservecount);
        const token first = tokens.front();

        unsigned long int index = 0;
        unsigned long int stopsize = tokens.size();

        long int argtracker = 0;
        long int farg_max;

        /*// Accepted token types

        X - NO
        - - YES

        - 0 - NUM
        - 1 - OPERATOR
        X 2 - LEFT BRACKET
        X 3 - RIGHT BRACKET
        - 4 - FUNCTION 
        - 5 - VARIABLE - Tested with targets
        - 6 - R FUNC
        X 7 - Separator
        - 9 - String
        - 10 - Assign

        -   1000 - Special NULL (VAR)
        */

        std::vector<token> argpack; // To be used in functions
        std::vector<token> dtemp;

        std::vector<token> operatorsstack;

        goto forceRun; // At least run through loop once no matter the condition
        while(index < stopsize){
            forceRun:
            if(!inturrupt_exit_flag) throw std::string("");

            switch(tokens[index].type){

                // Accept NUM && String
                
                case 5:
                    for(token test: targets){
                        if(test.data == tokens[index].data){
                            token nubr(test.data, 1000);
                            evalstack.emplace_back(nubr);
                            ++index;
                            break;
                        }
                    }
                    break;
                case 9:
                case 0:
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    break;

                case 1:
                    if(tokens[index].data == "POS"){
                        ++index;
                        continue;
                    }else if(tokens[index].data == "NOT"){
                        if(evalstack.back().type == 1000 ||
                           evalstack.back().type == 1 ||
                           evalstack.back().type == 4){
                            evalstack.emplace_back(tokens[index]);
                            ++index;
                            continue;
                        }
                        lnot(evalstack);
                        ++index;
                        continue;
                    }
                    if(evalstack.end()[-2].type == 1000 ||
                        evalstack.back().type == 1000 ||
                        evalstack.end()[-2].type == 1 ||
                        evalstack.back().type == 1 ||
                        evalstack.end()[-2].type == 4 ||
                        evalstack.back().type == 4){
                        if(evalstack.back().type == 1000 ||
                            evalstack.back().type == 1 ||
                            evalstack.back().type == 4){
                            token tk(evalstack.back().data,1000);
                        }else{
                            token tk(evalstack.end()[-2].data,1000);
                        }
                        evalstack.emplace_back(tokens[index]);
                        ++index;
                        continue;
                    }
                    ExecOperators[tokens[index].data](evalstack);
                    ++index;


                break;

            // Standard Function
            // Can handle Variadic Functions
            // Function Overloading is supported
            case 4:{
                std::string funcname = tokens[index].data;
                farg_max = tokens[index].reserved;
                if(!(fexists(funcname, farg_max) || funcname == freserved || f_isspecial(funcname, farg_max) || f_iscorefunc(funcname, farg_max))){
                    std::string error = "No matching function call to \"";
                    error.append(funcname).append("\"");
                    throw error;
                }

                while(!evalstack.empty() && argtracker < farg_max){
                    if(evalstack.back().type == 1000 ||
                       evalstack.back().type == 1 ||
                        evalstack.back().type == 4) dumpfunction = true;
                    if(evalstack.back().type == 0){
                        argpack.emplace_back(evalstack.back());
                        evalstack.pop_back();
                        argtracker++;
                    }else{
                        break;
                    }
                }

                // Check if recursive function with 0 arg - freserved
                if(dumpfunction || (funcname == freserved && farg_max == 0) || f_iscorefunc(funcname, farg_max)){
                    dumpfunction = false;
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    continue;
                }
        
                std::reverse(argpack.begin(),argpack.end());

                token tk;
                tk.type = 0;
                if(f_isspecial(funcname, farg_max)){
                    try{
                        tk.data = to_string_hprec(callspecial(argpack, funcname));
                    }catch(const int& e){
                        // Error
                    }
                }else{
                    tk.data = calculate(comp::retfillallvars(call(argpack, funcname)), false);
                }
                
                evalstack.emplace_back(tk);
                
                std::vector<token>().swap(argpack);
                argtracker = 0;
                ++index;
                }break;
            // SEP is ignored (Should not be here)
            case 7:{
                ++index;
            }break;

            default:{

                // Always increment index
                ++index;

                }break;
            }
        }

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;
        for(unsigned long int replaceidx = 0; replaceidx < evalstack.size(); replaceidx++){
            if(evalstack[replaceidx].type == 1000){
                evalstack[replaceidx].type = 5;
            }
        }
        for(token op: operatorsstack){
            evalstack.emplace_back(op);
        }
        operatorsstack.clear();
        tokens.clear();
        targets.clear();

        std::vector<token>().swap(operatorsstack);
        std::vector<token>().swap(tokens);
        std::vector<token>().swap(targets);

        return evalstack;
    }


    std::string calculate(std::vector<token> tokens, bool showprogress, unsigned long int reservecount){
        if(recursionCount > maxRecurse){
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        ++recursionCount;

        std::vector<token> evalstack;
        evalstack.reserve(reservecount);
        const token first = tokens.front();

        unsigned long int index = 0;
        unsigned long int stopsize = tokens.size();

        long int argtracker = 0;
        long int farg_max;

        bool varout = true;

        /*// Accepted token types
        - 0 - NUM
        - 1 - OPERATOR
        X 2 - LEFT BRACKET
        X 3 - RIGHT BRACKET
        - 4 - FUNCTION
        - 5 - VARIABLE
        - 6 - R FUNC
        X 7 - Separator
        - 9 - String
        - 10 - Assign
        */

        std::vector<token> argpack; // To be used in functions
        std::vector<token> dtemp;

        goto forceRun; // At least run through loop once no matter the condition
        while(index < stopsize){
            forceRun:

            if(showprogress) bar::setcount(index);
            if(!inturrupt_exit_flag) throw std::string("");

            switch(tokens[index].type){

                // Accept NUM && String, VARIABLE should not be at this stage, will cause error
                case 9:
                case 5: // Variable accepted anyways
                case 0:
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    break;

                case 1:
                case 10:
                    ExecOperators[tokens[index].data](evalstack);
                    ++index;
                    break;

                // Standard Function
                // Function Overloading is supported
                case 4:{ 
                    std::string funcname = tokens[index].data;
                    farg_max = tokens[index].reserved;
                    if(!(fexists(funcname, farg_max) || f_isspecial(funcname, farg_max) || f_iscorefunc(funcname, farg_max))){
                        std::string error = "No matching function call to \"";
                        error.append(funcname).append("\"");
                        throw error;
                    }

                    while(!evalstack.empty() && argtracker < farg_max){
                        if(evalstack.back().type == 0 || evalstack.back().type == 9){
                            argpack.emplace_back(evalstack.back());
                            evalstack.pop_back();
                            argtracker++;
                        }else{
                            break;
                        }
                    }

                    std::reverse(argpack.begin(),argpack.end());
                    token tk;
                    tk.type = 0;
                    if(f_isspecial(funcname, farg_max)){
                        try{
                            tk.data = to_string_hprec(callspecial(argpack, funcname));
                        }catch(const int& e){
                            // Error
                        }
                    }else if(f_iscorefunc(funcname, farg_max)){
                        varout = false;
                        tk = callcore(argpack, funcname);   
                    
                    }else{
                        tk.data = calculate(comp::retfillallvars(call(argpack, funcname)), false);
                    }
                    
                    evalstack.emplace_back(tk);

                    std::vector<token>().swap(argpack);
                    argtracker = 0;
                    ++index;
                    }break;

                // SEP is ignored (Should not be here)
                case 7:{
                    ++index;
                }break;
                default:{

                    // Always increment index
                    ++index;

                    }break;
            }
            if(evalstack.back().type == -1 && evalstack.size() > 0){
                evalstack.pop_back();
            }
        }

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;

        if(evalstack.size() <= 0 && varout){
            // If empty, return the most recently changed variable
            // Only assigning operators take everything
            return var::mostrecent();

        }

        // Success
        std::string strout;
        strout.append(evalstack.front().data);
        for(unsigned long int sdx = 1; sdx < evalstack.size(); sdx++){
            strout.append(evalstack[sdx].data);
            if(sdx + 1 < evalstack.size()) strout.append("\n");
        }

        tokens.clear();
        evalstack.clear();

        std::vector<token>().swap(tokens);
        std::vector<token>().swap(evalstack);

        return strout;
    }
}
